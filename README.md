# tpane

Works like `tail` but instead of interleaving the watched files, it shows each in a separate pane!

Very basic. Updates every 500ms, follows files by name rather than inodes (kinda), can only read lines less than 1024 characters (who knows what would happen), supports globbing, and likely has drawing errors!

Use it like `tail -f`: `tpane file1.log file2.log`

[![asciicast](https://asciinema.org/a/19m5v8at59917pryc8ubgz0tr.png)](https://asciinema.org/a/19m5v8at59917pryc8ubgz0tr)
