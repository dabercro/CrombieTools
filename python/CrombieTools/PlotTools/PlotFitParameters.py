from .. import Load

newFitPlotter = Load('PlotFitParameters')
plotter       = newFitPlotter()

def setupResolution(aPlotter=plotter, same_mean=True):
    aPlotter.SetLooseFit('[2] * TMath::Gaus(x,[0],[1])')

    second_mean = '[0]' if same_mean else '[0] - [5]'

    first_gaussian = '[4]*[3]/[1]*TMath::Gaus(x,[0],[1])'
    second_gaussian = '[4]*(1 - [3])/[2]*TMath::Gaus(x,%s,[2])' % second_mean

    aPlotter.SetFunction('%s + %s' % (first_gaussian, second_gaussian))
    aPlotter.AddFunctionComponent(first_gaussian)
    aPlotter.AddFunctionComponent(second_gaussian)

    aPlotter.AddMapping(0, 0)
    aPlotter.AddMapping(1, 1, 0.8)
    aPlotter.AddMapping(1, 2, 1.0)
    aPlotter.AddMapping(2, 4, 10)

def newResolutionPlotter():
    aPlotter = newFitPlotter()
    return setupResolution(aPlotter)

def SetParameterLimits(plotter, is_loose, *args):
    func = plotter.SetLooseLimits if is_loose else plotter.SetParameterLimits
    for index, limits in enumerate(args):
        if limits:
            func(index, *limits)

def SetGuesses(plotter, is_loose, *args):
    func = plotter.SetLooseGuess if is_loose else plotter.SetInitialGuess
    for index, guess in enumerate(args):
            func(index, guess)
