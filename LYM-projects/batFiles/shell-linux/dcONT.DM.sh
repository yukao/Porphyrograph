#!/usr/bin/perl
# composition of foreground line drawing and background
# quads

# line drawing based on corrected texture coordinates
# and quad orientation
# can be considered as the basis for any drawing algorithm

use File::stat;
use Time::localtime;

$dir="pic";

chdir("/home/christian/3D/LYM/");

if(-d $dir){
    $date_string = ctime(stat($dir)->mtime);
    $date_string =~ tr/ /_/;
    print "file $file updated at $date_string\n";

    system("mv $dir ${dir}_${date_string}");
}
mkdir $dir or die;

system("optirun DM dcONT/XML/LYM_dcONT.conf dcONT/XML/LYM_scene_dcONT.xml");
