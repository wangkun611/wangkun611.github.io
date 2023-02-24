#!/usr/bin/env python
import os
import pathlib
import subprocess
from optparse import OptionParser

def ConvertFile(base, src, dest):
    suffix = pathlib.Path(src).suffix
    if suffix in [".html", "htm"]:
        pathlib.Path(dest).mkdir(parents=True, exist_ok=True)
        dest = os.path.join(dest, pathlib.Path(src).name)
        subprocess.run(["cp", src, dest]) 
        return [dest]
    elif suffix in [".MD", ".md"]:
        pathlib.Path(dest).mkdir(parents=True, exist_ok=True)
        dest = os.path.join(dest, pathlib.Path(src).name.replace(suffix, ".html"))
        subprocess.run(
            ["pandoc", 
              "-s", "-p", "--wrap=none",
              "--toc", "-o", dest,
              src])
        return [dest]
    else:
        return [""]

def main(base, src, dest):
    htmlfiles = []
    for file in os.listdir(src):
        if os.path.isdir(file):
            if file[0] != '.':
                main(base, os.path.join(src, file), os.path.join(dest, file))
        else:
            html = ConvertFile(base, os.path.join(src, file), dest)[0]
            htmlfiles.append(html)
    htmlfiles = list(filter(lambda file: file != "", htmlfiles))
    if len(htmlfiles):
        with open(os.path.join(dest, "index.html"), "w", encoding="utf-8") as f:
            fli = map(lambda file: "".join(["<li><a href=\"./",  pathlib.Path(file).name, "\">", pathlib.Path(file).name, "</a> <date></date></li>"]), htmlfiles)
            f.write("<ol>")
            f.write("".join(list(fli)))
            f.write("</ol>")

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-b", "--base", dest="basedir",
                    help="Directory")

    (options, args) = parser.parse_args()
    main(options.basedir, options.basedir, "./.site/")
