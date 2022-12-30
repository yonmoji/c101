import configparser
import sys

config = configparser.ConfigParser()
config.read(sys.argv[1] or '.env.ini')
section = sys.argv[2] or "DEFAULT"
ds = []
for key in config[section]:
  ds.append("'-D%s=\"%s\"'" % (key.upper(), config[section][key]))
print(' '.join(ds))
