#! /usr/bin/env python

import os
import sys
import logging
import multiprocessing
import argparse
import Queue

parser = argparse.ArgumentParser()
parser.add_argument('--input', dest='input', help='Input directory', required=True)
parser.add_argument('--output', dest='output', help='Output directory', required=True)
parser.add_argument('--cut', dest='cut', help='Cut to skim the files')
parser.add_argument('--nprocs', dest='nprocs', type=int, help='Number of processes to do the conversion', default=1)
parser.add_argument('--branchfiles', dest='files', nargs='+', help='Files with branch names to convert', required=True)

args = parser.parse_args()

import ROOT

import tensorflow as tf


def error(*msg):
    logging.error(*msg)
    exit()


class TTreeReader(object):
    def __init__(self, infile, branches):

        self.file = ROOT.TFile(infile)
        self.tree = self.file.events

        self.cut = ROOT.TTreeFormula(args.cut, args.cut, self.tree)

        names = [branch.split(' = ')[0] for branch in branches]
        exprs = [branch.split(' = ')[1] if len(name) < len(branch) else branch
                 for name, branch in zip(names, branches)]

        self.branch_formulas = \
            [(name, ROOT.TTreeFormula(expr, expr, self.tree))
             for name, expr in zip(names, exprs)]

        self.ievent = 0
        self.numevents = self.tree.GetEntries()

    def __del__(self):
        self.file.Close()

    def next(self):
        while True:
            self.ievent += 1

            if self.ievent == self.numevents:
                return False

            self.tree.GetEntry(self.ievent)

            if not self.ievent % 10000:
                logging.warning('%i/%i processed in %s',
                                self.ievent, self.numevents,
                                self.file.GetName())

            if self.cut.EvalInstance():
                return True

    def read(self):
        return tf.train.Example(
            features=tf.train.Features(
                feature={
                    name: tf.train.Feature(
                        float_list=tf.train.FloatList(
                            value=[form.EvalInstance()]
                        )
                    )
                    for name, form in self.branch_formulas
                })
            ).SerializeToString()


class ConversionThread(multiprocessing.Process):
    def __init__(self, number, in_queue, branches):
        super(ConversionThread, self).__init__(name='ConversionThread--%i' % number)
        self.in_queue = in_queue
        self.branches = branches

    def run(self):
        while True:
            try:
                nextfile = self.in_queue.get(timeout=1)
                base = nextfile.split('.')[0]

                logging.warning('Opening %s', nextfile)
                reader = TTreeReader(os.path.join(args.input, nextfile),
                                     branches=self.branches)

                with tf.python_io.TFRecordWriter(os.path.join(args.output, '%s.tfrecord' % base),
                                                 tf.python_io.TFRecordOptions(compression_type=tf.python_io.TFRecordCompressionType.GZIP)) as writer:

                    while reader.next():
                        writer.write(reader.read())

            except Queue.Empty:
                break


if __name__ == '__main__':

    if os.path.exists(args.output) and os.listdir(args.output):
        error('Output directory not empty')

    if not os.path.isdir(args.input):
        error('Input directory does not exist')

    for infile in args.files:
        if not os.path.exists(infile):
            error('Branch file %s does not exist', infile)

    logging.warning('Writing to directory %s', args.output)
    logging.warning('Reading directory %s', args.input)
    logging.warning('Branches are in %s', args.files)
    logging.warning('Cut is %s', args.cut)

    if not os.path.exists(args.output):
        os.mkdir(args.output)

    branches = [line.strip() for branches_file in args.files
                for line in open(branches_file, 'r') if line.strip()]

    in_queue = multiprocessing.Queue()

    for infile in os.listdir(args.input):
        logging.warning('Adding %s', infile)
        in_queue.put(infile)

    threads = [ConversionThread(i, in_queue, branches)
               for i in range(args.nprocs)]

    logging.warning('Running %i processes', args.nprocs)

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
