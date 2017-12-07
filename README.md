# Windows Console Minimizable to Tray

Console opens, you press minimize, it minimizes to tray, you click icon, it maximizes.

# Instructions

1. You will need mingw or VisualStudio ("Desktop Development with C++" component, not .Net).
2. Use "Developer Command Prompt for VS 2017" if you use VS, foundable via start menu.
3. `rc.exe` doesn't stand utf, so you will need to convert `resource.utf8.rc` to other format, `deutf.sh` to your rescue.

# Compile for VisualStudio

1. `rc.exe resource.rc`
2. `cl.exe main.c /link user32.lib shell32.lib gdi32.lib resource.res` (or just `compile.cmd`)
