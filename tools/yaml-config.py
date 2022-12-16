import os
import sys
import yaml
import collections.abc
import operator


Import("env", "projenv")


CONFIG_DEFAULT = os.getenv("CONFIG_DEFAULT","default.yaml")
CONFIG_DEFAULT_LOCAL = os.getenv("CONFIG_DEFAULT_LOCAL","local.yaml")
CONFIG_DIR = os.getenv("CONFIG_DIR","config")
CONFIG_VAR = os.getenv("CONFIG_VAR","conf")
CONFIG_PREFIX = os.getenv("CONFIG_PREFIX")
CONFIG_LOWERCASE = os.getenv("CONFIG_LOWERCASE")


# taken from: https://stackoverflow.com/questions/6027558/flatten-nested-dictionaries-compressing-keys
_FLAG_FIRST = object()
def flatten_dict(d, join=operator.add, lift=lambda x:(x,)):
    results = []
    def visit(subdict, results, partialKey):
        for k,v in subdict.items():
            newKey = lift(k) if partialKey==_FLAG_FIRST else join(partialKey,lift(k))
            if isinstance(v,collections.abc.Mapping):
                visit(v, results, newKey)
            else:
                results.append((newKey,v))
    visit(d, results, _FLAG_FIRST)
    return results


def prepare_conf(conf):
    if not conf:
        return {}
    c_flattened=flatten_dict(conf)
    res = {}
    for k,v in c_flattened:
        conf_val = "_".join(k)
        if not CONFIG_LOWERCASE:
            conf_val = conf_val.upper()
        if CONFIG_PREFIX:
            conf_val = f"{CONFIG_PREFIX}_{conf_val}"
        res[conf_val] = v
    return res


cfile = os.getenv(CONFIG_VAR)
cpath = f"{CONFIG_DIR}/{cfile}.yaml"
cpath_default_local = f"{CONFIG_DIR}/{CONFIG_DEFAULT_LOCAL}"

if os.path.exists(cpath):
    print(f"yaml-conf: using config {cpath}")
elif os.path.exists(cpath_default_local):
    print(f"yaml-conf: using default local {cpath_default_local}")
    cpath = cpath_default_local
else:
    cpath = f"{CONFIG_DIR}/{CONFIG_DEFAULT}"
    print(f"yaml-conf: using default {cpath}")
    if not os.path.exists(cpath):
        print(f"yaml-conf: default config file does not exists: {cpath}")
        sys.exit(1)
try:
    with open(cpath, "r") as f:
        conf = yaml.safe_load(f)
except:
    print("yaml-conf: cannot load yaml config")
    sys.exit(1)

conf = prepare_conf(conf)

defines=[]
for k in conf:
    v = conf[k]
    if  isinstance(v, str):
        v = "\\\"" + v + "\\\""
    defines.append((k,v))

projenv.Append(CPPDEFINES=defines)
