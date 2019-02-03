cd D:\sync.com\Sync\D-Desktop\LYM_Annika_speakDance_2018\Shell
perl CopyLastSvgFromSnapshots.pl
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 1 DancerC.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathC.svg
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 2 DancerD.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathD.svg
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 3 DancerS.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathS.svg
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 4 DancerG.png white false D:\sync.com\Sync\LYM-projects\Data\core-data\SVGs\KompartSD\PathG.svg
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 1 DancerC.png white true .\PathAndDancerC.svg
magick convert -background transparent .\PathAndDancerC.svg .\PathAndDancerC.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 2 DancerD.png white true .\PathAndDancerD.svg
magick convert -background transparent .\PathAndDancerD.svg .\PathAndDancerD.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 3 DancerS.png white true .\PathAndDancerS.svg
magick convert -background transparent .\PathAndDancerS.svg .\PathAndDancerS.png
perl ShowOnlyOnePathAndAddPhoto.pl snapshotFullPerformance.svg 4 DancerG.png white true .\PathAndDancerG.svg
magick convert -background transparent .\PathAndDancerG.svg .\PathAndDancerG.png
magick convert -background transparent .\PathAndDancer4.svg .\PathAndDancer4.png
magick convert .\PathAndDancerC.png -extent 2048x2048 .\PathAndDancerC-extended.png
magick convert .\PathAndDancerD.png -extent 2048x2048 .\PathAndDancerD-extended.png
magick convert .\PathAndDancerG.png -extent 2048x2048 .\PathAndDancerS-extended.png
magick convert .\PathAndDancerS.png -extent 2048x2048 .\PathAndDancerG-extended.png
magick convert .\PathAndDancer4.png -extent 2048x2048 .\PathAndDancer4-extended.png
magick convert .\PathAndDancerC-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\PathAndDancerC.dds
magick convert .\PathAndDancerD-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\PathAndDancerD.dds
magick convert .\PathAndDancerS-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\PathAndDancerS.dds
magick convert .\PathAndDancerG-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\PathAndDancerG.dds
magick convert .\PathAndDancer4-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\PathAndDancer4.dds
copy .\PathAndDancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_005\PathAndDancerC_000.dds
copy .\PathAndDancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_005\PathAndDancerC_001.dds
copy .\PathAndDancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_006\PathAndDancerD_000.dds
copy .\PathAndDancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_006\PathAndDancerD_001.dds
copy .\PathAndDancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_007\PathAndDancerS_000.dds
copy .\PathAndDancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_007\PathAndDancerS_001.dds
copy .\PathAndDancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_008\PathAndDancerG_000.dds
copy .\PathAndDancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_008\PathAndDancerG_001.dds
copy .\PathAndDancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_009\PathAndDancer4_000.dds
copy .\PathAndDancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_009\PathAndDancer4_001.dds

magick convert .\DancerC.png -extent 2048x2048 .\DancerC-extended.png
magick convert .\DancerD.png -extent 2048x2048 .\DancerD-extended.png
magick convert .\DancerS.png -extent 2048x2048 .\DancerS-extended.png
magick convert .\DancerG.png -extent 2048x2048 .\DancerG-extended.png
magick convert .\Dancer4.png -extent 2048x2048 .\Dancer4-extended.png
magick convert .\DancerC-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\DancerC.dds
magick convert .\DancerD-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\DancerD.dds
magick convert .\DancerS-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\DancerS.dds
magick convert .\DancerG-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\DancerG.dds
magick convert .\Dancer4-extended.png -define dds:compression=dxt1 -define dds:mipmaps=0 .\Dancer4.dds
copy .\DancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_000\DancerC_000.dds
copy .\DancerC.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_000\DancerC_001.dds
copy .\DancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_001\DancerD_000.dds
copy .\DancerD.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_001\DancerD_001.dds
copy .\DancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_002\DancerS_000.dds
copy .\DancerS.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_002\DancerS_001.dds
copy .\DancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_003\DancerG_000.dds
copy .\DancerG.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_003\DancerG_001.dds
copy .\Dancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_004\Dancer4_000.dds
copy .\Dancer4.dds D:\sync.com\Sync\LYM-projects\Data\core-data\images\KompartSD\KompartSD_images_004\Dancer4_001.dds
cd D:\sync.com\Sync\LYM-projects\batFiles\