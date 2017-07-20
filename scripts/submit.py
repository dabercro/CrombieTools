#! /usr/bin/python


import os
import time
import sys
import logging
import shutil
import subprocess
import MySQLdb

import CrombieTools.LoadConfig

from collections import defaultdict

logging.basicConfig(level=logging.INFO)


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
            logging.warning('File: %s does not exists. Consider removing %s from sample list', file_list, sample)
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
                total_events = file_name[1]
            else:
                input_files += ',%s' % file_name[0]
                total_events += file_name[1]

        add_job()

    # Get all the new jobs and output directories from the database and return them
    curs.execute("SELECT id, output_dir, output_file FROM queue WHERE status = 'new'")
    jobs = [(int(row[0]), row[1], row[2]) for row in curs.fetchall()]

    conn.commit()
    conn.close()

    return jobs


def prepare_for_submit(jobs):

    # Check that the tarball exists
    tarball = os.path.join(os.environ['CMSSW_BASE'], 'condor.tgz')
    if not os.path.exists(tarball):
        logging.error('%s is missing. Make sure you make this.', tarball)
        exit(3)

    # Get locations of condor config, and html path
    local_condor = os.path.join(os.environ['PWD'], 'condor')
    html_path = os.path.join(os.environ['HOME'], 'public_html/submit')
    if not os.path.exists(local_condor):
        os.mkdir(local_condor)

    # Copy the condor config template and update it
    shutil.copy2(os.path.join(os.environ['CROMBIEPATH'],
                              'SubmitTools/condor/condor.cfg'),
                 local_condor)

    condor_cfg_name = os.path.join(local_condor, 'condor.cfg')

    with open(condor_cfg_name, 'a') as condor_cfg:
        condor_cfg.write('Executable = %s\n' % \
                             os.path.join(os.environ['CROMBIEPATH'],
                                          'SubmitTools/condor/run.sh'))
        condor_cfg.write('transfer_input_files = %s\n' % tarball)

        # Set the output log locations and the file mapping
        for job, output_dir, output_name in jobs:
            log_dir = os.path.join(html_path, 'logs', output_dir.split('/')[-1])

            if not os.path.exists(log_dir):
                os.makedirs(log_dir)

            condor_cfg.write('Output = %s\n' % os.path.join(log_dir, '%i.out' % job))
            condor_cfg.write('Error = %s\n' % os.path.join(log_dir, '%i.err' % job))
            condor_cfg.write('transfer_output_files = %s\n' % output_name)
            condor_cfg.write('transfer_output_remaps = "%s = %s"\n' % \
                                 (output_name, os.path.join(output_dir, output_name)))
            condor_cfg.write('Arguments = %i\nQueue\n' % job)

    return condor_cfg_name


def submit(config_file):

    # Return the exit code from the condor_submit.
    # Check that the configuration file exists first.
    if os.path.exists(config_file):
        return os.system('condor_submit %s' % config_file)
    return 1


def report_submission(jobs):
    conn, curs = connect()

    for job, _, _ in jobs:
        curs.execute("UPDATE queue SET status = 'submitted' WHERE id = %s", job)

    conn.commit()
    conn.close()


def check_jobs(jobs):

    # Check to see if any of the submitted jobs are still needing to be run
    # Pop out jobs that are finished

    conn, curs = connect()
    
    condor_q = "condor_q " + os.environ['USER'] + " {0} -format '%s\n' Args"
    def get_job_list(constraint):

        proc = subprocess.Popen([condor_q.format(constraint)],
                                stdout=subprocess.PIPE,
                                shell=True)

        stdout, _ = proc.communicate()

        return [int(id.strip()) for id in stdout.split('\n') \
                    if id.strip()]

    held_jobs = get_job_list("-constraint 'JobStatus == 5'")
    running_jobs = get_job_list("-constraint 'JobStatus != 4'")

    # Take care of all the completed jobs
    for job in jobs:
        if job[0] in running_jobs:
            continue

        output_file = os.path.join(job[1], job[2])
        if os.path.exists(output_file):

            curs.execute("SELECT num_events FROM queue WHERE id = ?", job[0])
            num_events = curs.fetchone()

            if subprocess.check_call(['crombie findtree --class TH1F --verify',
                                      num_events[0], output_file],
                                     shell=True) == 0:

                curs.execute("UPDATE queue SET status = 'finished' WHERE id = %s", job[0])
                jobs.pop(jobs.index(job))

            else:
                curs.execute("UPDATE queue SET status = 'failed' WHERE id = %s", job[0])


    # Remove the held jobs
    resub = []
    for job in jobs:
        if job[0] in held_jobs:
            resub.append(job)
            curs.execute("UPDATE queue SET status = 'failed' WHERE id = %s", job[0])

    if submit(prepare_for_submit(resub)) == 0:
        report_submission(resub)

    conn.commit()
    conn.close()


def check_bad_files():
    conn, curs = connect()

    samples = defaultdict(set)

    curs.execute('SELECT file_name, status FROM check_these')

    # Verify the report
    for file_name, status in curs.fetchall():
        verified = False

        if status == 'missing' and not os.path.exists(file_name):
            verified = True

        if (status == 'corrupt') and (subprocess.check_call(['crombie findtree', file_name],
                                                            shell=True) != 0):
            verified = True
            os.remove(file_name)

        split_name = file_name.split('/')
        # Get the book name and add the dataset
        samples['/'.join(split_name[-4:-2])].add(split_name[-2])

    for book, datasets in samples.iteritems():
        for dataset in datasets:
            print subprocess.check_call(
                'echo "y" | /home/cmsprod/DynamicData/SmartCache/Client/requestSample.sh %s %s' % (book, dataset),
                shell=True)

    curs.execute('DELETE FROM check_these')
    conn.commit()
    conn.close()


if __name__ == '__main__':

    check_bad_files()
    jobs = add_samples_to_database(sys.argv[1])

    # If the submission works, report the submitted jobs
    if submit(prepare_for_submit(jobs)) == 0:
        report_submission(jobs)

    while jobs:
        time.sleep(250)
        check_bad_files()
        check_jobs(jobs)
        print jobs

    check_bad_files()
