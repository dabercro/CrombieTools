#! /usr/bin/env python

import sys
import logging
import struct

from argparse import ArgumentParser

argv = sys.argv[:]
sys.argv = sys.argv[:1]
from ROOT import TFile, TTreeFormula
sys.argv = argv

import numpy
import keras

class Trainer(object):
    def __init__(self, config, target):
        self.vars = [line.strip() for line in open(config, 'r') if not line.startswith('#')]

        self.targets = target.split('|')

        self.model = keras.models.Sequential(
            [keras.layers.Dense(32, input_shape=(len(self.vars), ))] +
            [keras.layers.Dense(32) for _ in xrange(3)] +
            [keras.layers.Dense(len(self.targets))]
            )
        self.model.compile('rmsprop', 'mean_squared_error')


    def fit(self, input_file, cut, num_events):
        """
        Do the fit
        """
        in_file = TFile(input_file)
        tree = in_file.events
        val_forms = [TTreeFormula(v, v, tree) for v in self.vars]
        target_forms = [TTreeFormula(t, t, tree) for t in self.targets]
        cut_form = TTreeFormula(cut, cut, tree) if cut else None

        reserve = num_events if num_events > 0 else tree.GetEntries()

        inputs = numpy.zeros((reserve, len(self.vars)))
        targets = numpy.zeros((reserve, len(self.targets)))

        # Set up the inputs
        numcut = 0
        for index, _ in enumerate(tree):
            event = index - numcut
            if event == num_events:
                break
            if cut_form and not cut_form.EvalInstance():
                numcut += 1
                continue

            if event % 10000 == 0:
                print 'Filling', str(float(event)/num_events * 100) + '%'

            for jndex, val in enumerate(val_forms):
                inputs[event][jndex] = val.EvalInstance()

            for jndex, target in enumerate(target_forms):
                targets[event][jndex] = target.EvalInstance()

        self.model.fit(inputs, targets, validation_split=0.5)


if __name__ == '__main__':

    parser = ArgumentParser(
        prog='crombie keras',
        description = 'Reads some configuration file, takes some arguments and does Keras stuff'
        )

    parser.add_argument('-c', '--config', dest='config', metavar='FILE', help='The config file that contains the names of training variables')
    parser.add_argument('-i', '--input', dest='input', metavar='FILE', help='The name of the input file for tree')
    parser.add_argument('-x', '--cut', dest='cut', metavar='EXPR', help='The cut that events must pass to be trained on', default='')
    parser.add_argument('-t', '--target', dest='target', metavar='EXPR', help='The expression we want the regression for. Separate multiple targets with "|"')
    parser.add_argument('-o', '--output', dest='output', metavar='FILE', help='The name of the output file, which will be overwritten by training')
    parser.add_argument('-n', '--numevents', dest='numevents', metavar='NUM', type=int, help='The maximum number of events to train on (default all)', default=-1)

    args = parser.parse_args()

    model = Trainer(args.config, args.target)
    model.fit(args.input, args.cut, args.numevents)
