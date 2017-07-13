#! /usr/bin/python

import os
import sys
import MySQLdb

import CrombieTools.LoadConfig

if __name__ == '__main__':

    with open(sys.argv[1], 'r') as sample_file:
        samples = [line.strip() for line in sample_file if line.strip()]

        conn = MySQLdb.connect(read_default_file='/home/dabercro/my.cnf',
                               read_default_group='mysql-submit',
                               db='submit_queue')
        curs = conn.cursor()

    for sample in samples:
        file_list = os.path.join(
            '/home/cmsprod/catalog/t2mit',
            os.environ['CrombieInSample'], sample, 'Files'
            )

        if not os.path.exists(file_list):
            print 'File: %s does not exists. Consider removing %s from sample list' % (file_list, sample)
            continue

        with open(file_list, 'r') as file_file:
            files = [line.split()[1] for line in file_file if line.strip()]

        num_output = 0
        input_files = ''

        def add_job():
            if input_files:
                curs.execute(
                    """
                    INSERT INTO queue (exe, input_dir, output_dir, input_files, output_file, cmssw, attempts, entered)
                    VALUES (%s, %s, %s, %s, %s, %s, 0, NOW())
                    """, (
                        os.environ['CrombieExe'],
                        os.path.join('/store/user/paus', os.environ['CrombieInSample'], sample),
                        os.path.join(os.environ['CrombieOutDir'], sample.split('+')[0]),
                        input_files,
                        '%s.root' % str(num_output).zfill(4),
                        os.environ['CMSSW_BASE']
                        )
                    )

        for index, file_name in enumerate(files):
            if index % int(os.environ['CrombieFilesPerJob']) == 0:
                add_job()
                num_output += 1
                input_files = file_name
            else:
                input_files += ',%s' % file_name

        add_job()

    conn.commit()
    conn.close()
