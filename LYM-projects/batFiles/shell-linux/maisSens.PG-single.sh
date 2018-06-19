#!/usr/bin/perl

use File::stat;
use Time::localtime;

$homedir="/home/christian/3D/LYM/";

chdir("$homedir");

system("./cmd/PG_configAndHeaderGenerator.prl maisSens/LYM_maisSens-head-single.conf maisSens/LYM_maisSens.csv maisSens/LYM_maisSens.conf");

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

system("nice -19 primusrun Porphyrograph/bin/PG_maisSens maisSens/LYM_maisSens.conf");
