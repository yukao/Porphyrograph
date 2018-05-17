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

system("./cmd/generateVertices.prl CityWalk/XML/LYM_CW_HD.conf > CityWalk/XML/vertices.xml");
system("nice -n 19 optirun DM-1.8 CityWalk/XML/LYM_CW_HD.conf CityWalk/XML/LYM_scene_CW.xml");
