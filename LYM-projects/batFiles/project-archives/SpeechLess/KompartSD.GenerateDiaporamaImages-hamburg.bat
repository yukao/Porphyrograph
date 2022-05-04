cd D:\sync.com\Sync\LYM-works\LYM_Annika_speakDance_2018

copy .\DancerImages\Dancer10.png .\RecordReplayData\Images\
copy .\DancerImages\Dancer11.png .\RecordReplayData\Images\
copy .\DancerImages\Dancer12.png .\RecordReplayData\Images\
copy .\DancerImages\Dancer13.png .\RecordReplayData\Images\

magick convert .\RecordReplayData\Images\Dancer10.png -extent 2048x2048 .\RecordReplayData\Images\Dancer10-extended.png
magick convert .\RecordReplayData\Images\Dancer11.png -extent 2048x2048 .\RecordReplayData\Images\Dancer11-extended.png
magick convert .\RecordReplayData\Images\Dancer12.png -extent 2048x2048 .\RecordReplayData\Images\Dancer12-extended.png
magick convert .\RecordReplayData\Images\Dancer13.png -extent 2048x2048 .\RecordReplayData\Images\Dancer13-extended.png

magick convert .\RecordReplayData\Images\Dancer10-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\Dancer10.dds
magick convert .\RecordReplayData\Images\Dancer11-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\Dancer11.dds
magick convert .\RecordReplayData\Images\Dancer12-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\Dancer12.dds
magick convert .\RecordReplayData\Images\Dancer13-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\Dancer13.dds

mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_000\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_001\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_002\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_003\

copy .\RecordReplayData\Images\Dancer10.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_000\Dancer10_000.dds
copy .\RecordReplayData\Images\Dancer10.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_000\Dancer10_001.dds
copy .\RecordReplayData\Images\Dancer11.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_001\Dancer11_000.dds
copy .\RecordReplayData\Images\Dancer11.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_001\Dancer11_001.dds
copy .\RecordReplayData\Images\Dancer12.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_002\Dancer12_000.dds
copy .\RecordReplayData\Images\Dancer12.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_002\Dancer12_001.dds
copy .\RecordReplayData\Images\Dancer13.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_003\Dancer13_000.dds
copy .\RecordReplayData\Images\Dancer13.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_003\Dancer13_001.dds

perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 1 .\Dancer10.png white true .\RecordReplayData\Images\PathAndDancer10.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer10.svg .\RecordReplayData\Images\PathAndDancer10.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 2 .\Dancer11.png white true .\RecordReplayData\Images\PathAndDancer11.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer11.svg .\RecordReplayData\Images\PathAndDancer11.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 3 .\Dancer12.png white true .\RecordReplayData\Images\PathAndDancer12.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer12.svg .\RecordReplayData\Images\PathAndDancer12.png
perl D:\sync.com\Sync\LYM-sources\RecordReplay\ShowOnlyOnePathAndAddPhoto.pl .\RecordReplayData\Images\snapshotFullPerformance.svg 4 .\Dancer13.png white true .\RecordReplayData\Images\PathAndDancer13.svg
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer13.svg .\RecordReplayData\Images\PathAndDancer13.png
magick convert -background transparent .\RecordReplayData\Images\PathAndDancer14.svg .\RecordReplayData\Images\PathAndDancer14.png

magick convert .\RecordReplayData\Images\PathAndDancer10.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer10-extended.png
magick convert .\RecordReplayData\Images\PathAndDancer11.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer11-extended.png
magick convert .\RecordReplayData\Images\PathAndDancer12.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer12-extended.png
magick convert .\RecordReplayData\Images\PathAndDancer13.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer13-extended.png
magick convert .\RecordReplayData\Images\PathAndDancer14.png -extent 2048x2048 .\RecordReplayData\Images\PathAndDancer14-extended.png

magick convert .\RecordReplayData\Images\PathAndDancer10-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer10.dds
magick convert .\RecordReplayData\Images\PathAndDancer11-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer11.dds
magick convert .\RecordReplayData\Images\PathAndDancer12-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer12.dds
magick convert .\RecordReplayData\Images\PathAndDancer13-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer13.dds
magick convert .\RecordReplayData\Images\PathAndDancer14-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\RecordReplayData\Images\PathAndDancer14.dds

mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_005\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_006\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_007\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_008\
mkdir D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_009\

copy .\RecordReplayData\Images\PathAndDancer10.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_005\PathAndDancer10_000.dds
copy .\RecordReplayData\Images\PathAndDancer10.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_005\PathAndDancer10_001.dds
copy .\RecordReplayData\Images\PathAndDancer11.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_006\PathAndDancer11_000.dds
copy .\RecordReplayData\Images\PathAndDancer11.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_006\PathAndDancer11_001.dds
copy .\RecordReplayData\Images\PathAndDancer12.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_007\PathAndDancer12_000.dds
copy .\RecordReplayData\Images\PathAndDancer12.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_007\PathAndDancer12_001.dds
copy .\RecordReplayData\Images\PathAndDancer13.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_008\PathAndDancer13_000.dds
copy .\RecordReplayData\Images\PathAndDancer13.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_008\PathAndDancer13_001.dds
copy .\RecordReplayData\Images\PathAndDancer14.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_009\PathAndDancer14_000.dds
copy .\RecordReplayData\Images\PathAndDancer14.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD-hamburg\KompartSD_images_009\PathAndDancer14_001.dds

cd D:\sync.com\Sync\LYM-projects\batFiles\