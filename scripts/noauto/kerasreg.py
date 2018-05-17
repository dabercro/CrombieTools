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

from tensorflow.python.framework import graph_util
from tensorflow.python.framework import graph_io

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s:%(levelname)s: %(message)s')

class Trainer(object):
    def __init__(self, config, target, layers, weights):
        self.vars = [line.strip() for line in open(config, 'r') if not line.startswith('#')]
        self.targets = target.split('|')

        logging.info('Using %i variables', len(self.vars))
        logging.info('Training for %i targets', len(self.targets))

        nodes_in_layer = int(len(self.vars)/2)

        input_layer = keras.layers.Input(shape=(len(self.vars), ), name='inputs')
        layer = input_layer
        for _ in xrange(layers):
            layer = keras.layers.Dense(nodes_in_layer)(layer)

        outputs = [keras.layers.Activation(
                activation='linear', name='out/%i' % index)(keras.layers.Dense(1)(layer)) \
                       for index in range(len(self.targets))]
        self.model = keras.models.Model(inputs=input_layer, output=outputs)
        self.model.compile('adam', 'mae',
                           loss_weights=[int(w) for w in weights.split('|')] if weights else None)


    def fit(self, input_file, cut, num_events, output, epochs, batch):
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
        targets = [numpy.zeros((reserve, 1)) for _ in self.targets]

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
                print 'Filling', str(float(event * 100)/num_events) + '%'

            for jndex, val in enumerate(val_forms):
                inputs[event][jndex] = val.EvalInstance()

            for jndex, target in enumerate(target_forms):
                targets[jndex][event][0] = target.EvalInstance()

        self.model.fit(inputs, targets, validation_split=0.5, epochs=epochs, batch_size=batch)

        sess = keras.backend.get_session()
        output_node = [n.op.name for n in self.model.outputs]
        print 'Output node', output_node
        graph = graph_util.convert_variables_to_constants(sess, sess.graph.as_graph_def(), output_node)

        graph_io.write_graph(graph, 'weights', output, as_text=False)


if __name__ == '__main__':

    parser = ArgumentParser(
        prog='crombie kerasreg',
        description = 'Reads some configuration file, takes some arguments and does Keras stuff'
        )

    parser.add_argument('-c', '--config', dest='config', metavar='FILE', help='The config file that contains the names of training variables')
    parser.add_argument('-i', '--input', dest='input', metavar='FILE', help='The name of the input file for tree')
    parser.add_argument('-x', '--cut', dest='cut', metavar='EXPR', help='The cut that events must pass to be trained on', default='')
    parser.add_argument('-t', '--target', dest='target', metavar='EXPR', help='The expression we want the regression for. Separate multiple targets with "|"')
    parser.add_argument('-o', '--output', dest='output', metavar='FILE', help='The name of the output file, which will be overwritten by training')
    parser.add_argument('-n', '--numevents', dest='numevents', metavar='NUM', type=int, help='The maximum number of events to train on (default all)', default=-1)
    parser.add_argument('-e', '--epochs', dest='epochs', metavar='NUM', type=int, help='Number of epochs', default=10)
    parser.add_argument('-b', '--batch', dest='batch', metavar='NUM', type=int, help='Batch size', default=1000)
    parser.add_argument('-l', '--layers', dest='layers', metavar='NUM', type=int, help='Number of hidden layers', default=5)
    parser.add_argument('-w', '--weights', dest='weights', metavar='STR', help='Weights for each target ("|" separated)', default='')

    args = parser.parse_args()

    model = Trainer(args.config, args.target, args.layers, args.weights)
    model.fit(args.input, args.cut, args.numevents, args.output, args.epochs, args.batch)
