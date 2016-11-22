# Two dictionaries to define the cuts for separate categories and control regions

categoryCuts = {
    'example' : '1'
    }


regionCuts = {
    'signal' : 'exampleDisc1 > 0.5',
    'unblinded' : 'exampleDisc1 > 0.5',
    'uncorrected' : 'exampleDisc1 > 0.5',
    'control' : 'exampleDisc1 < 0.5 && isSignal == 0',
    }

# These are just for the users to loop over

categories = categoryCuts.keys()
regions    = regionCuts.keys()

# Making selection of multiple entries

def joinCuts(toJoin=regionCuts.keys(), cuts=regionCuts):
    return ' && '.join([cuts[cut] for cut in toJoin])

# A weight applied to all MC

defaultMCWeight = 'allWeights * reweight'

# Additional weights applied to certain control regions

region_weights    = { # key : [Data,MC]
    'signal'      : ['0', defaultMCWeight],
    'uncorrected' : ['1', 'weight'],
    'control'     : ['1', 'allWeights'],
    'default'     : ['1', defaultMCWeight]
    }

# Do not change the names of these functions or required parameters
# Otherwise you cannot use some convenience functions
# Multiple regions are concatenated with '+'
# Generally you can probably leave these alone

def cut(category, region):
    return '((' + categoryCuts[category] + ') && (' + joinCuts(toJoin=region.split('+')) + '))'

def dataMCCuts(region, isData):
    key = 'default'
    if region in region_weights.keys():
        key = region

    index = 0 if isData else 1

    return '(' + region_weights[key][index] + ')'
