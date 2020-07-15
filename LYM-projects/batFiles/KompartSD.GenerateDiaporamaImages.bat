cd D:\sync.com\Sync\LYM-works\LYM_Annika_speakDance_2018

perl D:\sync.com\Sync\LYM-sources\RecordReplay\CopyLastFileFromDir.pl D:\sync.com\Sync\LYM-projects\snapshots svg .\RecordReplayData\Images\snapshotFullPerformance.svg

copy .\DancerImages\DancerC.png .\RecordReplayData\Images\
copy .\DancerImages\DancerD.png .\RecordReplayData\Images\
copy .\DancerImages\DancerS.png .\RecordReplayData\Images\
copy .\DancerImages\DancerG.png .\RecordReplayData\Images\
copy .\DancerImages\Dancer4.png .\RecordReplayData\Images\

magick convert .\RecordReplayData\Images\DancerC.png -extent 2048x2048 .\RecordReplayData\Images\DancerC-extended.png
magick convert .\RecordReplayData\Images\DancerD.png -extent 2048x2048 .\RecordReplayData\Images\DancerD-extended.png
magick convert .\RecordReplayData\Images\DancerS.png -extent 2048x2048 .\RecordReplayData\Images\DancerS-extended.png
magick convert .\RecordReplayData\Images\DancerG.png -extent 2048x2048 .\RecordReplayData\Images\DancerG-extended.png
magick convert .\RecordReplayData\Images\Dancer4.png -extent 2048x2048 .\RecordReplayData\Images\Dancer4-extended.png

magick convert .\RecordReplayData\Images\DancerC-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\DancerC.dds
magick convert .\RecordReplayData\Images\DancerD-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\DancerD.dds
magick convert .\RecordReplayData\Images\DancerS-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\DancerS.dds
magick convert .\RecordReplayData\Images\DancerG-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\DancerG.dds
magick convert .\RecordReplayData\Images\Dancer4-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\Dancer4.dds

copy .\RecordReplayData\Images\DancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_000\DancerC_000.dds
copy .\RecordReplayData\Images\DancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_000\DancerC_001.dds
copy .\RecordReplayData\Images\DancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_001\DancerD_000.dds
copy .\RecordReplayData\Images\DancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_001\DancerD_001.dds
copy .\RecordReplayData\Images\DancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_002\DancerS_000.dds
copy .\RecordReplayData\Images\DancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_002\DancerS_001.dds
copy .\RecordReplayData\Images\DancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_003\DancerG_000.dds
copy .\RecordReplayData\Images\DancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_003\DancerG_001.dds
copy .\RecordReplayData\Images\Dancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_004\Dancer4_000.dds
copy .\RecordReplayData\Images\Dancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_004\Dancer4_001.dds

REM perl ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 1 .\RecordReplayData\Images\DancerC.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathC.svg
REM perl ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 2 .\RecordReplayData\Images\DancerD.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathD.svg
REM perl ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 3 .\RecordReplayData\Images\DancerS.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathS.svg
REM perl ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 4 .\RecordReplayData\Images\DancerG.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathG.svg

perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 1 .\DancerC.png white true .\RecordReplayData\Images\PathAndDancerC.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancerC.svg .\RecordReplayData\Images\PathAndDancerC.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 2 .\DancerD.png white true .\RecordReplayData\Images\PathAndDancerD.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancerD.svg .\RecordReplayData\Images\PathAndDancerD.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 3 .\DancerS.png white true .\RecordReplayData\Images\PathAndDancerS.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancerS.svg .\RecordReplayData\Images\PathAndDancerS.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 4 .\DancerG.png white true .\RecordReplayData\Images\PathAndDancerG.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancerG.svg .\RecordReplayData\Images\PathAndDancerG.png
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer4.svg .\RecordReplayData\Images\PathAndDancer4.png

magick convert .\RecordReplayData\Images\PathAndDancerC.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancerC-extended.png
magick convert .\RecordReplayData\Images\PathAndDancerD.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancerD-extended.png
magick convert .\RecordReplayData\Images\PathAndDancerS.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancerS-extended.png
magick convert .\RecordReplayData\Images\PathAndDancerG.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancerG-extended.png
magick convert .\RecordReplayData\Images\PathAndDancer4.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer4-extended.png

magick convert .\RecordReplayData\Images\PathAndDancerC-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancerC.dds
magick convert .\RecordReplayData\Images\PathAndDancerD-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancerD.dds
magick convert .\RecordReplayData\Images\PathAndDancerS-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancerS.dds
magick convert .\RecordReplayData\Images\PathAndDancerG-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancerG.dds
magick convert .\RecordReplayData\Images\PathAndDancer4-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer4.dds

copy .\RecordReplayData\Images\PathAndDancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_005\PathAndDancerC_000.dds
copy .\RecordReplayData\Images\PathAndDancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_005\PathAndDancerC_001.dds
copy .\RecordReplayData\Images\PathAndDancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_006\PathAndDancerD_000.dds
copy .\RecordReplayData\Images\PathAndDancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_006\PathAndDancerD_001.dds
copy .\RecordReplayData\Images\PathAndDancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_007\PathAndDancerS_000.dds
copy .\RecordReplayData\Images\PathAndDancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_007\PathAndDancerS_001.dds
copy .\RecordReplayData\Images\PathAndDancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_008\PathAndDancerG_000.dds
copy .\RecordReplayData\Images\PathAndDancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_008\PathAndDancerG_001.dds
copy .\RecordReplayData\Images\PathAndDancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_009\PathAndDancer4_000.dds
copy .\RecordReplayData\Images\PathAndDancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_009\PathAndDancer4_001.dds

cd D:\sync.com\Sync\LYM-projects\batFiles\