#! /usr/bin/env python

import sys
import logging

from argparse import ArgumentParser

argv = sys.argv[:]
sys.argv = sys.argv[:1]
from ROOT import TMVA, TFile, TCut
sys.argv = argv

import CrombieTools

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s:%(levelname)s: %(message)s')

TMVA.Tools.Instance()


class Trainer(object):
    def __init__(self, signal_file, back_file, regression_file, output_file, name, options):
        # All of these are "private". Please don't touch directly
        self.signal_file = TFile(signal_file) if signal_file else None
        self.back_file = TFile(back_file) if back_file else None
        self.input_file = TFile(regression_file) if regression_file else None
        self.output_file = TFile(output_file, 'RECREATE')
        self.loader = TMVA.DataLoader(name)
        self.factory = TMVA.Factory(name, self.output_file, options)
        self.weight = None
        self.cut = None
        self.variables = None   # Flag that variables have been set
        self.target = None      # Flag that target has been set
        self.prepared = None    # Flag that trees have been prepared

    def __del__(self):
        for f in [self.input_file, self.signal_file, self.back_file]:
            if f:
                f.Close()
        self.output_file.Close()
        del self.factory

    def read_var_config(self, config):
        self.variables = True
        with open(config, 'r') as config_file:
            for line in config_file:
                if line.startswith('#'):
                    continue

                inputs = line.strip().split()
                if not inputs:
                    continue

                target_func = self.loader.AddVariable
                if inputs[0] == 'SPEC':
                    target_func = self.loader.AddSpectator
                    inputs = inputs[1:]

                if len(inputs) == 1:
                    target_func(inputs[0])
                elif len(inputs) == 2:
                    target_func('%s:=%s' % (inputs[0], inputs[1]))

    def set_weight(self, weight):
        self.weight = weight

    def set_cut(self, cut):
        self.cut = TCut(cut)

    def set_target(self, name, expr):
        self.target = True
        for n, e in zip(name.split('|'), expr.split('|')):
            self.loader.AddTarget('%s:=%s' % (n, e))

    def prepare(self, opts):
        if None in [self.cut, self.weight]:
            logging.error('Cut or weight is not set')
            exit(1)

        self.prepared = True

        if self.input_file:
            self.loader.AddRegressionTree(self.input_file.Get('events'), 1.0)
            self.loader.SetWeightExpression(self.weight, 'Regression');
            self.loader.PrepareTrainingAndTestTree(self.cut, opts)
        else:
            self.loader.AddSignalTree(self.signal_file.Get('events'), 1.0)
            self.loader.AddBackgroundTree(self.back_file.Get('events'), 1.0)
            self.loader.SetSignalWeightExpression(self.weight)
            self.loader.SetBackgroundWeightExpression(self.weight)
            self.loader.PrepareTrainingAndTestTree(self.cut, self.cut, opts)

    # Static member
    methods = {
        'Variable': TMVA.Types.kVariable,
        'Cuts': TMVA.Types.kCuts,
        'Likelihood': TMVA.Types.kLikelihood,
        'PDERS': TMVA.Types.kPDERS,
        'HMatrix': TMVA.Types.kHMatrix,
        'Fisher': TMVA.Types.kFisher,
        'KNN': TMVA.Types.kKNN,
        'CFMlpANN': TMVA.Types.kCFMlpANN,
        'TMlpANN': TMVA.Types.kTMlpANN,
        'BDT': TMVA.Types.kBDT,
        'DT': TMVA.Types.kDT,
        'RuleFit': TMVA.Types.kRuleFit,
        'SVM': TMVA.Types.kSVM,
        'MLP': TMVA.Types.kMLP,
        'BayesClassifier': TMVA.Types.kBayesClassifier,
        'FDA': TMVA.Types.kFDA,
        'Boost': TMVA.Types.kBoost,
        'PDEFoam': TMVA.Types.kPDEFoam,
        'LD': TMVA.Types.kLD,
        'Plugins': TMVA.Types.kPlugins,
        'Category': TMVA.Types.kCategory,
        'DNN': TMVA.Types.kDNN,
        'PyRandomForest': TMVA.Types.kPyRandomForest,
        'PyAdaBoost': TMVA.Types.kPyAdaBoost,
        'PyGTB': TMVA.Types.kPyGTB,
        'PyKeras': TMVA.Types.kPyKeras,
        'C50': TMVA.Types.kC50,
        'RSNNS': TMVA.Types.kRSNNS,
        'RSVM': TMVA.Types.kRSVM,
        'RXGB': TMVA.Types.kRXGB,
        'MaxMethod': TMVA.Types.kMaxMethod
        }

    def add_method(self, method, name, opts):
        if not name:
            name = method

        self.factory.BookMethod(self.loader, self.methods[method], name, opts)

    def train(self):
        self.factory.TrainAllMethods()
        self.factory.TestAllMethods()
        self.factory.EvaluateAllMethods()


if __name__ == '__main__':

    parser = ArgumentParser(
        prog='crombie tmva',
        description = 'Reads some configuration file, takes some arguments and does TMVA stuff'
        )

    parser.add_argument('-c', '--config', dest='config', metavar='FILE', help='The config file that contains the names of training variables')
    parser.add_argument('-r', '--regression', dest='regression', metavar='FILE', help='The name of the input file for regression tree', default='')
    parser.add_argument('-s', '--signal', dest='signal', metavar='FILE', help='The name of the input file for signal tree', default='')
    parser.add_argument('-b', '--background', dest='background', metavar='FILE', help='The name of the input file for background tree', default='')
    parser.add_argument('-w', '--weight', dest='weight', metavar='EXPR', help='The weight of the events')
    parser.add_argument('-x', '--cut', dest='cut', metavar='EXPR', help='The cut that events must pass to be trained on')
    parser.add_argument('-t', '--target', dest='target', metavar='EXPR', help='The expression we want the regression for. Separate multiple targets with "|"', default='')
    parser.add_argument('-m', '--method', dest='method', metavar='NAME', help='Method to use in training')
    parser.add_argument('-o', '--methodopt', dest='method_opts', metavar='OPTS', help='Options for method')

    parser.add_argument('-p', '--prepare', dest='prepare', metavar='OPTS', help='The options for preparing the test and training trees')

    parser.add_argument('--output', dest='output', metavar='FILE', help='The name of the output file, which will be overwritten by training', default='TMVA.root')
    parser.add_argument('--methodname', dest='method_name', metavar='NAME', help='What to name the method the BDT output', default='')
    parser.add_argument('--trainername', dest='trainer_name', metavar='NAME', help='Name of the trainer', default='TMVA')
    parser.add_argument('--traineropt', dest='trainer_opts', metavar='OPTS', help='Options for the trainer', default='!V:!Silent:Color:DrawProgressBar')
    parser.add_argument('--targetname', dest='target_name', metavar='NAME', help='The name of the target', default='target')

    args = parser.parse_args()

    trainer = Trainer(args.signal, args.background, args.regression, args.output, args.trainer_name, args.trainer_opts)

    trainer.read_var_config(args.config)

    trainer.set_weight(args.weight)
    trainer.set_cut(args.cut)
    if args.target:
        trainer.set_target(args.target_name, args.target)
    trainer.prepare(args.prepare)
    trainer.add_method(args.method, args.method_name, args.method_opts)
    trainer.train()

    del trainer
