categories = ['example']
regions    = ['signal']

categoryCuts = {
    'example' : '1'
    }

regionCuts = {
    'signal' : '1'
    }

def cut(category, region):
    return '((' + categoryCuts[category] + ') && (' + regionCuts[region] + '))'
