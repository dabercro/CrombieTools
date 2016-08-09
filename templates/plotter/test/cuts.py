# Two dictionaries to define the cuts for separate categories and control regions

categoryCuts = {
    'example' : '1'
    }


regionCuts = {
    'signal' : '1',
    'unblinded' : '1',
    }

# These are just for the users to loop over

categories = categoryCuts.keys()
regions    = regionCuts.keys()

# Making selection of multiple entries

def joinCuts(toJoin=regionCuts.keys(), cuts=regionCuts):
    return ' && '.join([cuts[cut] for cut in toJoin])

# A weight applied to all MC

defaultMCWeight = 'allWeights'

# Additional weights applied to certain control regions

additions    = { # key : [Data,MC]
    'signal'  : ['0','1'],
    'default' : ['1',defaultMCWeight]
    }

# Do not change the names of these functions or required parameters
# Otherwise you cannot use some convenience functions
# Multiple regions are concatenated with '+'
# Generally you can probably leave these alone

def cut(category, region):
    return '((' + categoryCuts[category] + ') && (' + joinCuts(toJoin=region.split('+')) + '))'

def dataMCCuts(region, isData):
    key = 'default'
    if region in additions.keys():
        key = region

    index = 1
    if isData:
        index = 0

    if key == 'default' or index == 0:
        return '(' + additions[key][index] + ')'
    else:
        return '((' + additions[key][index] + ')*(' + defaultMCWeight + '))'
