#! /usr/bin/env python

import os
import sys
import sqlite3
import math
import logging

def setup_table(curs, table_name):
    """
    Add a predefined table to the output file.
    Definitions are in this function here.
    """

    curs.execute('DROP TABLE IF EXISTS {0}'.format(table_name))

    creation_commands = {
        'links': """
process VARCHAR(64),
region VARCHAR(64),
connect_to VARCHAR(64)
""",
        'uncertainties': """
bin INT,
process VARCHAR(64),
region VARCHAR(64),
value DOUBLE,
shape VARCHAR(64),
source VARCHAR(64)
""",
        'correlations': """
unc_0 INT,
unc_1 INT,
value DOUBLE
""",
        }

    curs.execute(
        'CREATE TABLE %s (id INT PRIMARY KEY, %s)' % \
            (table_name, creation_commands[table_name]))

    return table_name


def add_to_table(curs, table_name, *args):

    if not table_name:
        print 'Table not set!'
        exit(10)

    line = sum(args, [])

    logging.debug(line)

    curs.execute(
        'INSERT INTO {0} VALUES ({1})'.format(
            table_name, ', '.join(['?'] * len(line))),
        line)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print 'Usage: %s INPUT OUTPUT\n' % sys.argv[0]
        print 'Reads information on links and systematic uncertainties'
        print 'from INPUT and adds them to the database in the OUTPUT file.'
        exit(1)

    logging.basicConfig(level=int(os.environ.get('DEBUG', logging.INFO)))

    conn = sqlite3.connect(sys.argv[2])
    curs = conn.cursor()

    current = ''
    index = 0

    with open(sys.argv[1], 'r') as input_file:
        for line in input_file:
            # Skip commented lines
            if line[0] == '#':
                continue

            contents = line.split()
            if not contents:
                continue

            if contents[0] == 'BEGIN':
                current = setup_table(curs, contents[1])
                index = 0
            elif contents[0] == 'LINK':
                # LINK gives a list of uncertainties to add to the link ratios

                curs.execute("SELECT DISTINCT(region) FROM links WHERE connect_to = '-'")

                query = """
                        SELECT yields.bin, yields.process, yields.region, {0}
                        FROM yields
                        INNER JOIN links ON links.process = yields.process AND links.region = yields.region
                        INNER JOIN yields connect ON links.connect_to = connect.process and connect.region = '{1}' and connect.bin = yields.bin
                        WHERE yields.contents > 0.0
                        """.format(', '.join(['{0}.{1}/{0}.contents'.format(table, '/{0}.contents, {0}.'.format(table).join(contents[1:])) \
                                                  for table in ['yields', 'connect']]),
                                   curs.fetchone()[0])

                logging.debug(query)

                curs.execute(query)

                for from_yields in curs.fetchall():
                    index += 1
                    logging.debug(from_yields)
                    add_to_table(curs, current, [index],
                                 list(from_yields)[:3],
                                 [math.sqrt(sum([x*x for x in from_yields[3:]]))],
                                 ['gaussian', 'ratio_unc'])

            elif ('0' in contents or '*' in contents) and 'GLOBAL' not in contents:
                constraints = ['type = "background"']
                if contents[0] != '0':
                    constraints.append('bin = %i' % contents[0])
                if contents[1] != '*':
                    constraints.append('process = "%s"' % contents[1])
                if contents[2] != '*':
                    constraints.append('region = "%s"' % contents[2])

                where_clause = 'WHERE {0}'.format(' AND '.join(constraints))

                query = """
                        SELECT bin, process, region FROM yields {0};
                        """.format(where_clause)

                logging.debug(query)

                curs.execute(query)

                for which_bin in curs.fetchall():
                    index += 1
                    logging.debug(which_bin)
                    add_to_table(curs, current, [index],
                                 list(which_bin),
                                 contents[3:])

            else:
                # Inserting one line at a time isn't the most efficient,
                # but I don't have enough entries to care
                if contents[0] == 'GLOBAL':
                    contents.pop(0)

                index += 1
                add_to_table(curs, current, [index], contents)

    conn.commit()
    conn.close()
