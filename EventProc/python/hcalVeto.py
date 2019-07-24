
from LDMX.Framework import ldmxcfg 

hcalVeto = ldmxcfg.Producer("HcalVeto", "ldmx::HcalVetoProcessor")
hcalVeto.parameters['pe_threshold'] = 5.0
hcalVeto.parameters['max_time'] = 50.0
hcalVeto.parameters['max_depth'] = 4000.0
hcalVeto.parameters['back_min_pe'] = 1.
