#! /usr/bin/python


import os
import sys
import logging
import shutil
import MySQLdb

import CrombieTools.LoadConfig


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
            files = [line.split()[1] for line in file_file if line.strip()]

        # Define a function that will separate the files and insert them into the submission queue
        input_files = ''

        def add_job():
            if input_files:
                output_dir = sample.split('+')[0]
                output_files[output_dir] = output_files.get(output_dir, 0) + 1
                curs.execute(
                    """
                    INSERT IGNORE INTO queue
                    (exe, input_dir, output_dir, input_files, output_file, cmssw, entered, scram_arch, base)
                    VALUES (%s, %s, %s, %s, %s, %s, NOW(), %s, %s)
                    """,
                    (os.environ['CrombieExe'],
                     os.path.join('/store/user/paus', os.environ['CrombieInSample'], sample),
                     os.path.join(os.environ['CrombieOutDir'], output_dir),
                     input_files,
                     '%s.root' % str(output_files[output_dir]).zfill(4),
                     os.environ['CMSSW_VERSION'],
                     os.environ['SCRAM_ARCH'],
                     os.environ['CMSSW_BASE']
                     )
                    )

        # Split the files and add a job into the database when needed
        for index, file_name in enumerate(files):
            if index % int(os.environ['CrombieFilesPerJob']) == 0:
                add_job()
                input_files = file_name
            else:
                input_files += ',%s' % file_name

        add_job()

    # Get all the new jobs and output directories from the database and return them
    curs.execute("SELECT id, output_dir, output_file FROM queue WHERE status = 'new'")
    jobs = [(row[0], row[1], row[2]) for row in curs.fetchall()]

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
    pass


def report_submission(jobs):
    conn, curs = connect()

    for job in jobs:
        curs.exectute("UPDATE queue SET status = 'submitted' WHERE id = %i", job)

    conn.commit()
    conn.close()


if __name__ == '__main__':

    jobs = add_samples_to_database(sys.argv[1])
    submit(prepare_for_submit(jobs))
    #report_submission(jobs)
    #while still_running(jobs):
    #    time.sleep(600)
