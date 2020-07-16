#!/usr/bin/perl 

use strict;
use warnings;

# text_to_image.pl
# text into images via SVG model

# usage
# text_to_image.pl -o didascalie.png  -i didascalies.txt

# IN
#
#              mp4 video

# OUT 
#
#              mp4 posterized video

my $USAGE = "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/TextToImage/text_to_image.pl -o didascalie.png  -i didascalies.txt -m didascalie-model.svg

ARGUMENTS:
   -i: input TXT file
   -o: ouput PNG file
   -m: model SVG file
";

my $input_file_name;
my $output_file_name;
my $output_file_name_wo_extension;
my $model_file_name;

##################################################################
# COLOR MIXTURE
##################################################################
if( @ARGV != 6 ) {
    print $USAGE; die"text_to_image.pl: number of arguments: 6 not ", 0-@ARGV;
}

# output_file_name
if( $ARGV[0] ne "-o" ) {
    print $USAGE; die"text_to_image.pl: -o expected for argt 1";
}
$output_file_name = $ARGV[1];
$output_file_name_wo_extension = $output_file_name;
$output_file_name_wo_extension =~ s/\.png//;

# input_file_name
if( $ARGV[2] ne "-i" ) {
    print $USAGE; die"text_to_image.pl: -i expected for argt 3";
}
$input_file_name = $ARGV[3];

# model_file_name
if( $ARGV[4] ne "-m" ) {
    print $USAGE; die"text_to_image.pl: -m expected for argt 5";
}
$model_file_name = $ARGV[5];

my $FILEout;
my $FILEin;
my $FILEmodelIn;
my $FILEtmpOut;
open($FILEout , ">Didascalies/$output_file_name" ) 
  || die "Can't open file Didascalies/$output_file_name!";
open($FILEin , "<Didascalies/$input_file_name") 
  || die "Can't open file Didascalies/$input_file_name!";

# transforms vector inmages back into bitmaps
my $ind_file = 0;
my $line = <$FILEin>;
my $firstLine = "";
my $secondLine = "";
while(1){
  chomp($line);
  # print "\n\nline: (", $line, ")\n";
  if( !$line || $line =~ /\-/ ) {
    last;
  }
  if( $line =~ /\+/ ) {
    $firstLine = <$FILEin>;
    $secondLine = <$FILEin>;
    chop($firstLine);
    chop($secondLine);
    if( $secondLine =~ /\+/ || $secondLine =~ /\-/ ) {
      $line = $secondLine;
      $secondLine = "";
    }
    else {
      $line = <$FILEin>;
    }
    if(!( $line =~ /\+/ || $line =~ /\-/ )) {
      die "Unexpected $line in file $model_file_name!";
    }
    # print "firstLine: (", $firstLine, ")\n";
    # print "secondLine: (", $secondLine, ")\n";

    open($FILEmodelIn , "<./Didascalies/".$model_file_name) 
      || die "Can't open file ".$model_file_name."!";
    open($FILEtmpOut , ">./Didascalies/tmp.svg" ) 
      || die "Can't open file $FILEtmpOut!";
    my $lineTmpOut;
    while($lineTmpOut = <$FILEmodelIn>){
      if( $lineTmpOut =~ /TXT1/ ) {
        $lineTmpOut =~ s/TXT1/$firstLine/;
      }
      elsif( $lineTmpOut =~ /TXT2/ ) {
        $lineTmpOut =~ s/TXT2/$secondLine/;
      }
      print $FILEtmpOut $lineTmpOut;
    }
    close($FILEmodelIn);
    close($FILEtmpOut);
    my $outputImageFileName = sprintf "$output_file_name_wo_extension%04d.png", $ind_file;
    my $outputExtendedImageFileName = sprintf "$output_file_name_wo_extension%04d-extended.png", $ind_file;
    my $outputDdsImageFileName1 = sprintf "$output_file_name_wo_extension%04d_000.dds", $ind_file;
    my $outputDdsImageFileName2 = sprintf "$output_file_name_wo_extension%04d_001.dds", $ind_file;

    print "\ninkscape -z -w 1920 -h 1080 --export-area-page -b #000000 -e ./Didascalies/$outputImageFileName ./Didascalies/tmp.svg","\n";
    system( "inkscape -z -w 1920 -h 1080 --export-area-page -b #000000 -e ./Didascalies/$outputImageFileName ./Didascalies/tmp.svg" ); 

    print "convert ./Didascalies/$outputImageFileName -extent 2048x2048 ./Didascalies/$outputExtendedImageFileName","\n";
    system( "convert ./Didascalies/$outputImageFileName -extent 2048x2048 ./Didascalies/$outputExtendedImageFileName" ); 

    print "convert ./Didascalies/$outputExtendedImageFileName -define dds:compression=dxt1 -define dds:mipmaps=0 ./Didascalies/$outputDdsImageFileName1","\n";
    system( "convert ./Didascalies/$outputExtendedImageFileName -define dds:compression=dxt1 -define dds:mipmaps=0 ./Didascalies/$outputDdsImageFileName1" ); 

    my $imageDir = sprintf "/mnt/d/sync.com/Sync/LYM-projects/Data/core-data/images/Tempete/Tempete_images_%03d", $ind_file;
    if(!(-d $imageDir)) {
      print "mkdir $imageDir","\n";
      system( "mkdir $imageDir" ); 
    }

    print "cp -f ./Didascalies/$outputDdsImageFileName1 $imageDir/$outputDdsImageFileName2","\n";
    system( "cp -f ./Didascalies/$outputDdsImageFileName1 $imageDir/$outputDdsImageFileName2" ); 
    print "mv -f ./Didascalies/$outputDdsImageFileName1 $imageDir/","\n";
    system( "mv -f ./Didascalies/$outputDdsImageFileName1 $imageDir/" ); 

    $ind_file++;
  }
}
print "rm -f ./Didascalies/*png ./Didascalies/tmp.svg","\n";
system( "rm -f ./Didascalies/*png  ./Didascalies/tmp.svg" ); 
close($FILEin);
close($FILEout);
