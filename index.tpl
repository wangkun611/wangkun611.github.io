<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" lang="$lang$" xml:lang="$lang$"$if(dir)$ dir="$dir$"$endif$>
<head>
  <meta charset="utf-8" />
  <meta name="generator" content="pandoc" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=yes" />
  <title>$if(title-prefix)$$title-prefix$ – $endif$$title$</title>
$for(header-includes)$
  $header-includes$
$endfor$
  <!--[if lt IE 9]>
    <script src="//cdnjs.cloudflare.com/ajax/libs/html5shiv/3.7.3/html5shiv-printshiv.min.js"></script>
  <![endif]-->
</head>
<body>
  <header class="index">
      <h1><a href="/">$site_title$</a></h1>
  </header>
  <ol id="articles" reversed>
  $for(articles)$
      <li><a href="/$it.path$">$it.title$</a> <date>$it.date$</date></li>
  $endfor$
  </ol>
</body>
</html>