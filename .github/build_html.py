#!/usr/bin/env python
import json
import os
import pathlib
import subprocess
from optparse import OptionParser
import tempfile
import frontmatter
import datetime  
  
class DateEncoder(json.JSONEncoder):  
    def default(self, obj):  
        if isinstance(obj, datetime.datetime):  
            return obj.strftime("%Y-%m-%d")  
        elif isinstance(obj, datetime.date):  
            return obj.strftime("%Y-%m-%d")  
        else:  
            return json.JSONEncoder.default(self, obj) 


'''
    把输入文件`src`转成html文件，存储到`dest`目录。`base`是项目的根目录。
    目前支持html，md文件
    返回JSON对象 或者 None
    json对象说明: 
     - title: 标题
     - path: 相对根目录的地址
     - date: 发布时间
'''
def ConvertFile(base, src, dest):
    name = pathlib.Path(src).name
    suffix = pathlib.Path(src).suffix
    relative = pathlib.Path(src).relative_to(base)
    if suffix in [".html", "htm"]:
        pathlib.Path(dest).mkdir(parents=True, exist_ok=True)
        dest = os.path.join(dest, name)
        subprocess.run(["cp", src, dest]) 
        return {
            "title": name.replace(suffix, "", -len(suffix))
        }
    elif suffix in [".MD", ".md"]:
        pathlib.Path(dest).mkdir(parents=True, exist_ok=True)
        dest = os.path.join(dest, pathlib.Path(src).name.replace(suffix, ".html"))
        subprocess.run(
            ["pandoc", 
              "-s", "-p", "--wrap=none",
              "--from=gfm",
              "--metadata-file", os.path.join(base, "metadata.yaml"),
              "--template", os.path.join(base, "article.tpl"),
              "--include-in-header", os.path.join(base, "header.tpl"),
              "--toc", "-o", dest,
              src])
        article = frontmatter.load(src)
        return {
            "title": article["title"],
            "date":  article["date"],
            "path": relative.as_posix().replace(suffix, ".html")
        }
    elif suffix in [".css", ".jpg", ".png", ".svg"]:
        pathlib.Path(dest).mkdir(parents=True, exist_ok=True)
        dest = os.path.join(dest, name)
        subprocess.run(["cp", src, dest]) 
        return None
    else:
        return None

def main(base, src, dest):
    htmlfiles = []
    for file in os.listdir(src):
        if os.path.isdir(file):
            if file[0] != '.':
                htmlfiles.extend(main(base, os.path.join(src, file), os.path.join(dest, file)))
        else:
            html = ConvertFile(base, os.path.join(src, file), dest)
            htmlfiles.append(html)
    htmlfiles = list(filter(lambda file: file != None and  file["title"] != "", htmlfiles))
    if len(htmlfiles):
        htmlfiles.sort(key=lambda ele: ele["date"], reverse=True)
        temp = tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", suffix=".yaml", delete=True)
        temp.write("articles:\n")
        for file in htmlfiles:
            temp.write("- ")
            temp.write(json.dumps(file, cls=DateEncoder))
            temp.write("\n")
        temp.flush()

        subprocess.run(
            ["pandoc", 
              "-s", "-p", "--wrap=none",
              "--from=gfm",
              "--metadata-file", os.path.join(base, "metadata.yaml"),
              "--template", os.path.join(base, "index.tpl"),
              "--include-in-header", os.path.join(base, "header.tpl"),
              "--toc", "-o", os.path.join(dest, "index.html"),
              "--metadata-file", temp.name,
              "/dev/null"])
    return htmlfiles

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-b", "--base", dest="basedir",
                    help="Directory")

    (options, args) = parser.parse_args()
    main(options.basedir, options.basedir, "./.site/")
