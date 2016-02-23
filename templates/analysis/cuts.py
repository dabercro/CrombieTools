categories = ['example']
regions    = ['signal']

categoryCuts = {
    'example' : '1'
    }

regionCuts = {
    'signal' : '1'
    }

additionKeys = ['signal']
additions    = { # key : [Data,MC]
    'signal'  : ['0','1'],
    'default' : ['1','1']
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

    return '(' + additions[key][index] + ')'
