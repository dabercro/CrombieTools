#! /usr/bin/python


import os
import time
import datetime
import sys
import logging
import shutil
import subprocess
import MySQLdb

import CrombieTools.LoadConfig

from collections import defaultdict

logging.basicConfig(level=logging.INFO)
LOG = logging.getLogger('squid')


def connect():
    conn = MySQLdb.connect(read_default_file='/home/dabercro/my.cnf',
                           read_default_group='mysql-submit',
                           db='submit_queue')
    curs = conn.cursor()

    return conn, curs


def add_samples_to_database(sample_file_name):

    # Get the list of samples from the input file
    # Samples should be MIT style (with '+'s)
    with open(sample_file_name, 'r') as sample_file:
        samples = [line.strip() for line in sample_file if line.strip()]

    conn, curs = connect()

    output_files = {}

    for sample in samples:

        # Get the file catalog
        file_list = os.path.join(
            '/home/cmsprod/catalog/t2mit',
            os.environ['CrombieInSample'], sample, 'Files'
            )

        if not os.path.exists(file_list):
            LOG.warning('File: %s does not exists. Consider removing %s from sample list', file_list, sample)
            continue

        with open(file_list, 'r') as file_file:
            files = [line.split()[1:3] for line in file_file if line.strip()]

        # Define a function that will separate the files and insert them into the submission queue
        input_files = ''
        total_events = 0

        def add_job():
            if input_files:
                output_dir = sample.split('+')[0]
                output_files[output_dir] = output_files.get(output_dir, 0) + 1

                full_out_dir = os.path.join(os.environ['CrombieOutDir'], output_dir)
                if not os.path.exists(full_out_dir):
                    os.makedirs(full_out_dir)

                curs.execute(
                    """
                    INSERT IGNORE INTO queue
                    (exe, input_dir, output_dir, input_files, output_file, cmssw, entered, scram_arch, base, total_events)
                    VALUES (%s, %s, %s, %s, %s, %s, NOW(), %s, %s, %s)
                    """,
                    (os.environ['CrombieExe'],
                     os.path.join('/store/user/paus', os.environ['CrombieInSample'], sample),
                     full_out_dir,
                     input_files,
                     '%s.root' % str(output_files[output_dir]).zfill(4),
                     os.environ['CMSSW_VERSION'],
                     os.environ['SCRAM_ARCH'],
                     os.environ['CMSSW_BASE'],
                     total_events
                     )
                    )

        # Split the files and add a job into the database when needed
        for index, file_name in enumerate(files):
            if index % int(os.environ['CrombieFilesPerJob']) == 0:
                add_job()
                input_files = file_name[0]
                total_events = int(file_name[1])
            else:
                input_files += ',%s' % file_name[0]
                total_events += int(file_name[1])

        add_job()

    # Get all the new jobs and output directories from the database and return them
    curs.execute("SELECT id, output_dir, output_file FROM queue WHERE status = 'new'")
    jobs = [(int(row[0]), row[1], row[2]) for row in curs.fetchall()]

    conn.commit()
    conn.close()

    return jobs


def prepare_for_submit(jobs):

    if not jobs:
        return ''

    # Check that the tarball exists
    tarball = os.path.join(os.environ['CMSSW_BASE'], 'condor.tgz')
    if not os.path.exists(tarball):
        LOG.error('%s is missing. Make sure you make this.', tarball)
        exit(3)

    # Get locations of condor config, and html path
    local_condor = os.path.join(os.environ['PWD'], 'condor')
    html_path = os.path.join(os.environ['HOME'], 'public_html/squid')
    if not os.path.exists(local_condor):
        os.mkdir(local_condor)

    # Copy the condor config template and update it
    shutil.copy2(os.path.join(os.environ['CROMBIEPATH'],
                              'SubmitTools/condor/condor.cfg'),
                 local_condor)

    condor_cfg_name = os.path.join(local_condor, 'condor.cfg')

    conn, curs = connect()

    with open(condor_cfg_name, 'a') as condor_cfg:
        condor_cfg.write('Executable = %s\n' % \
                             os.path.join(os.environ['CROMBIEPATH'],
                                          'SubmitTools/condor/run.sh'))
        condor_cfg.write('transfer_input_files = %s\n' % tarball)

        # Set the output log locations and the file mapping
        for job, output_dir, output_name in jobs:
            if os.path.exists(os.path.join(output_dir, output_name)):
                continue

            curs.execute('SELECT attempts FROM queue WHERE id = %s', job)
            attempt = curs.fetchone()[0]
            curs.execute('UPDATE queue SET attempts = %s WHERE id = %s', (attempt + 1, job))

            LOG.debug('Appending %s to condor submission file', job)

            log_dir = os.path.join(html_path, 'logs', 
                                   output_dir.split('/')[-2],
                                   output_dir.split('/')[-1])

            if not os.path.exists(log_dir):
                os.makedirs(log_dir)

            log_basename = os.path.join(log_dir, '%s_%i' % (output_name.split('.')[0], attempt))

            condor_cfg.write('Output = %s.out\n' % log_basename)
            condor_cfg.write('Error = %s.err\n' % log_basename)
            condor_cfg.write('transfer_output_files = %s\n' % output_name)
            condor_cfg.write('transfer_output_remaps = "%s = %s"\n' % \
                                 (output_name, os.path.join(output_dir, output_name)))
            condor_cfg.write('Arguments = %i\nQueue\n' % job)

    conn.commit()
    conn.close()

    return condor_cfg_name


def report_submission(jobs):
    conn, curs = connect()

    for job, _, _ in jobs:
        curs.execute("UPDATE queue SET status = 'submitted' WHERE id = %s", job)

    conn.commit()
    conn.close()


def submit(jobs):

    config_file = prepare_for_submit(jobs)

    # Return the exit code from the condor_submit.
    # Check that the configuration file exists first.
    if os.path.exists(config_file):
        report_submission(jobs)
        os.system('condor_submit %s' % config_file)
        LOG.info('Submitted jobs, sleeping')
        time.sleep(1200)


def check_jobs():
    # Check to see if any of the submitted jobs are still needing to be run
    # Pop out jobs that are finished

    conn, curs = connect()
    
    curs.execute("SELECT id, output_dir, output_file FROM queue WHERE status != 'finished'")
    jobs = [(int(row[0]), row[1], row[2]) for row in curs.fetchall()]

    condor_q = "condor_q " + os.environ['USER'] + " -format '%s\n' Args"
    proc = subprocess.Popen([condor_q],
                            stdout=subprocess.PIPE,
                            shell=True)

    stdout, _ = proc.communicate()
    running_jobs = [int(id.strip()) for id in stdout.split('\n') \
                        if id.strip().isdigit()]

    LOG.info('Checking %i jobs', len(jobs))

    # List for jobs to resubmit
    resub = []
    def add_resub(job):
        LOG.debug('Adding job %s to be resubmitted', job[0])
        resub.append(job)
        curs.execute("UPDATE queue SET status = 'failed' WHERE id = %s", job[0])
        
    finished_job = []

    for job in jobs:
        # If running, do nothing
        output_file = os.path.join(job[1], job[2])
        LOG.debug('Checking job %s at %s', job[0], output_file)

        if job[0] in running_jobs:
            LOG.debug('Job is running')
            continue

        if os.path.exists(output_file):

            curs.execute("SELECT total_events FROM queue WHERE id = %s", job[0])
            num_events = curs.fetchone()[0]

            try:
                LOG.debug('Checking %s has %s events', output_file, num_events)
                subprocess.check_call(['crombie', 'findtree', '--class', 'TH1F', '--verify',
                                       num_events, output_file])

                curs.execute("UPDATE queue SET status = 'finished' WHERE id = %s", job[0])
                LOG.info('Job %s finished', job[0])
                finished_job.append(job)

            except subprocess.CalledProcessError:
                LOG.warning('Bad file %s', output_file)
                os.remove(output_file)
                add_resub(job)
        else:
            LOG.warning('File %s does not exist!', output_file)
            add_resub(job)

    for job in finished_job:
        jobs.pop(jobs.index(job))


    conn.commit()
    conn.close()

    LOG.info('Total jobs to finish: %i', len(jobs))
    LOG.info('Already running: %i', len(running_jobs))
    LOG.info('To resubmit: %i', len(resub))

    submit(resub)

    if len(jobs) > (len(running_jobs) + len(resub)):
        return check_jobs()

    return len(jobs)


def check_bad_files():
    LOG.info('Checking bad files.')

    conn, curs = connect()

    samples = defaultdict(set)

    curs.execute('SELECT file_name, status FROM check_these')

    # Verify the report
    for file_name, status in curs.fetchall():
        continue         # Skip all the rest of this stuff for now
        verified = False

        source = file_name.replace('/mnt/hadoop/cms', '/mnt/hadoop/scratch/' + os.environ['USER'])

        # Corrupt files might be missing too if I interrupted this step earlier
        if status in ['missing', 'corrupt'] and not os.path.exists(file_name):
            verified = True

        # Only check existing corrupt files if they're old
        elif status == 'corrupt' and (time.time() - os.stat(file_name).st_mtime) > (3600 * 12):
        
            try:
                subprocess.check_call(['crombie', 'findtree', file_name])
            except subprocess.CalledProcessError:
                verified = True
                os.remove(file_name)

        if verified:
            if os.path.exists(source):
                shutil.move(source, file_name)
            else:
                split_name = file_name.split('/')
                # Get the book name and add the dataset
                samples['/'.join(split_name[-4:-2])].add(split_name[-2])

    for book, datasets in samples.iteritems():
        for dataset in datasets:
            subprocess.check_call(
                'echo "y" | /home/cmsprod/DynamicData/SmartCache/Client/requestSample.sh %s %s' % (book, dataset),
                shell=True)

    curs.execute('DELETE FROM check_these where status = "missing" or status = "corrupt"')
    conn.commit()
    conn.close()


if __name__ == '__main__':

    jobs = 1

    # Submit new jobs
    if len(sys.argv) > 1:
        # Make sure newest executable is installed
        os.system('make install')
        # Proxy in case we need to copy files
        os.system('voms-proxy-init -voms cms --valid 140:00')
        submit(add_samples_to_database(sys.argv[1]))

    while jobs:
        LOG.info('Starting submit cycle %s',
                 datetime.datetime.fromtimestamp(int(time.time())).strftime('%B %d, %Y %H:%M:%S'))
        # Check job output, which also resubmits jobs
        jobs = check_jobs()
        LOG.info('Jobs remaining: %i', jobs)
        LOG.info('Ended submit cycle %s',
                 datetime.datetime.fromtimestamp(int(time.time())).strftime('%B %d, %Y %H:%M:%S'))

        if jobs:
            time.sleep(300)
            # Remove held jobs
            os.system('condor_rm %s --constraint \'JobStatus == 5\'' % os.environ['USER'])
            # Check the panda files
            check_bad_files()
