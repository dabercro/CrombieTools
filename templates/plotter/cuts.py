# These are just for the users to loop over

categories = ['example']
regions    = ['signal']

# Two dictionaries to define the cuts for separate categories and control regions

categoryCuts = {
    'example' : '1'
    }

regionCuts = {
    'signal' : '1'
    }

# A weight applied to all MC

defaultMCWeight = 'mcWeight'

# Additional weights applied to certain control regions

additions    = { # key : [Data,MC]
    'signal'  : ['0','1'],
    'default' : ['1',defaultMCWeight]
    }

# Do not change the names of these functions or required parameters
# Otherwise you cannot use some convenience functions
# Generally you can probably leave these alone

def cut(category, region):
    return '((' + categoryCuts[category] + ') && (' + regionCuts[region] + '))'

def dataMCCuts(region, isData):
    key = 'default'
    if additions.Get(region):
        key = region

    index = 1
    if isData:
        index = 0

    if key == 'default' or index == 0:
        return '(' + additions[key][index] + ')'
    else:
        return '((' + additions[key][index] + ')*(' + defaultMCWeight + '))'
