cd D:\sync.com\Sync\D-Desktop\LYM_Annika_speakDance_2018\Shell
perl CopyLastSvgFromSnapshots.pl
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 1 DancerC.png red PathAndDancerC.svg
magick convert -background transparent .\PathAndDancerC.svg .\1_PathAndDancerC.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 2 DancerD.png blue PathAndDancerD.svg
magick convert -background transparent .\PathAndDancerD.svg .\2_PathAndDancerD.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 3 DancerS.png green PathAndDancerS.svg
magick convert -background transparent .\PathAndDancerS.svg .\3_PathAndDancerS.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 4 DancerG.png yellow PathAndDancerG.svg
magick convert -background transparent .\PathAndDancerG.svg .\4_PathAndDancerG.png
magick convert -background transparent .\PathAndDancer4.svg .\5_PathAndDancer4.png
move /y *_PathAndDancer*.png ..\Diaporama
cd D:\sync.com\Sync\LYM-projects\batFiles\