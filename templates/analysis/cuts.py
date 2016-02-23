categories = ['example']
regions    = ['signal']

categoryCuts = {
    'example' : '1'
    }

regionCuts = {
    'signal' : '1'
    }

defaultMCWeight = 'mcWeight'

additionKeys = ['signal']
additions    = { # key : [Data,MC]
    'signal'  : ['0','1'],
    'default' : ['1',defaultMCWeight]
    }

def cut(category, region):
    return '((' + categoryCuts[category] + ') && (' + regionCuts[region] + '))'

def dataMCCuts(region, isData):
    key = 'default'
    index = 1
    if region in additionKeys:
        key = region

    if isData:
        index = 0

    if key == 'default' or index == 0:
        return '(' + additions[key][index] + ')'
    else:
        return '((' + additions[key][index] + ')*(' + defaultMCWeight + '))'
