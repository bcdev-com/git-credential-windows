# git-credential-windows

I was inspired by Andrew Nurse's 
[gitstorecred](http://vibrantcode.com/blog/2012/4/16/gitstorecred-store-https-credentials-for-git.html) tool 
and Paul Betts [blog post](https://github.com/blog/1104-credential-caching-for-wrist-friendly-git-usage)
but I wanted something written in plain C that could potentially be merged into the standard msysGit distribution.

# pros and cons

Native code means no dependancies.   

No UI means it should work on XP and above.

No UI also means initial credential prompts are performed by git at the command line.  
This pleases my aesthetic sense, but won't be for everyone.

# contact

I can be reached via email at blake@bcdev.com or feel free to open an issue in this project.
