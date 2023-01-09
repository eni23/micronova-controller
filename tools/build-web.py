import os
import sys
import subprocess
Import("env", "projenv")


HTML_FILE = os.getenv("WEB_FILE","web/index.html")
HTML_FILE_OUT= os.getenv("WEB_FILE_HEADERNAME","web_content.h")

# check if web build is enabled
enabled=False
for k in projenv.Dictionary()['CPPDEFINES']:
  if k[0]=="WEB_SERVER_ENABLE" and k[1]:
    enabled = True
if not enabled:
  print(f"build-web: web feature is disabled")
  sys.exit(0)

print(f"build-web: building web")

# create build dir
pdir = projenv.Dictionary()['PROJECT_WORKSPACE_DIR']
wdir = f"{pdir}/web"
ofile = f"{wdir}/{HTML_FILE_OUT}"
if not os.path.exists(wdir):
  os.makedirs(wdir)

# check for "html-minify"
minifier_bin=False
try: 
  minifier_bin = subprocess.check_output(["which","html-minifier"],stderr=False).decode().strip()
except subprocess.CalledProcessError:
  print("build-web: http-minify not found, please install with 'npm install html-minifier -g' ")
  print("build-web: will use uncompressed html")

# do the easy conversion, no compression
if not minifier_bin:
  with open(HTML_FILE, 'r') as f:
    webcontent = f.read().replace("\n","")
# compress nice
else:
  print("build-web: using html-minifier")
  minify_cmd = [
    "html-minifier",
    "--collapse-whitespace", 
    "--remove-comments", 
    "--remove-optional-tags", 
    "--remove-redundant-attributes", 
    "--remove-script-type-attributes", 
    "--remove-tag-whitespace", 
    "--use-short-doctype", 
    "--minify-css=true", 
    "--minify-js=true",
    HTML_FILE
  ]
  webcontent = subprocess.check_output(minify_cmd).decode().strip()

# write file
webcontent = webcontent.replace("\"","\\\"")
headerfile = f"const char* web_index_html = \"{webcontent}\";"
with open(ofile, "w") as f:
  f.write(headerfile)

# append created source file to cpppath
projenv.Append(CPPPATH=wdir)





