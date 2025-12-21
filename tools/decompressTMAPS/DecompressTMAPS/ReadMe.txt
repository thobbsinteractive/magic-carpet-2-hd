This tool Decompresses the sprites from Magic Carpet 2 DAT files and saves them (by default) as PNGs with Transparency
To use this tool:
- 1: Copy the exe and dll files to C:\Program Files (x86)\GOG Galaxy\Games\Magic Carpet 2\CD_FILES\DATA
- 2: Run the exe with the following parameters:

DecompressTMAPS.exe -p "[Path to Pallet File] -t [Path to .DAT file (needs a .TAB file of the same name)] -i [Image type data/png/bmp/pnga (defaults to png)] -f [folder-pattern 1/0/2]"

e.g:
For the night level sprites:
DecompressTMAPS.exe -p PALN-0.DAT -t TMAPS1-0.DAT -f 1 -o out-night

For the day level sprites:
DecompressTMAPS.exe -p PALD-0.DAT -t TMAPS0-0.DAT -f 0 -o out-day

For cave level sprites:
DecompressTMAPS.exe -p PALC-0.DAT -t TMAPS2-0.DAT -f 2 -o out-cave

You can us DecompressTMAPS.exe -h to list these parameters are anytime

By default, Sprites will be decompressed to .\Out