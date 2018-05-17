#!/usr/bin/perl

use File::stat;
use Time::localtime;

$homedir="/home/christian/3D/LYM/";

chdir("$homedir");

system("./cmd/PG_configAndHeaderGenerator.prl GN/LYM_GN-head-single.conf GN/LYM_GN.csv GN/LYM_GN.conf");

$dir="pic";
if(-d $dir){
    $date_string = ctime(stat($dir)->mtime);
    $date_string =~ s/^[A-Za-z]+ //; # suppress day name
    $date_string =~ s/:[0-9]+ [A-Za-z ]*/ /; # suppress seconds + locale
    $date_string =~ tr/ /_/;
    $date_string =~ tr/:/_/;

    print "directory $dir updated at $date_string\n";
    system("mv $dir ${dir}_${date_string}");
}
mkdir $dir or die;

system("nice -19 primusrun Porphyrograph/bin/PG_GN GN/LYM_GN.conf");
