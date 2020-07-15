cd D:\sync.com\Sync\LYM-works\LYM_Annika_speakDance_2018

perl D:\sync.com\Sync\LYM-sources\RecordReplay\CopyLastFileFromDir.pl .\RecordReplayData\OSCcommands\ txt .\RecordReplayData\OSCreplay\replayFullPerformance.txt

perl D:\sync.com\Sync\LYM-sources\RecordReplay\RecordOnlyOneScene.pl .\RecordReplayData\OSCreplay\replayFullPerformance.txt 1 .\RecordReplayData\OSCreplay\scene1.txt
perl D:\sync.com\Sync\LYM-sources\RecordReplay\RecordOnlyOneScene.pl .\RecordReplayData\OSCreplay\replayFullPerformance.txt 2 .\RecordReplayData\OSCreplay\scene2.txt
perl D:\sync.com\Sync\LYM-sources\RecordReplay\RecordOnlyOneScene.pl .\RecordReplayData\OSCreplay\replayFullPerformance.txt 3 .\RecordReplayData\OSCreplay\scene3.txt
perl D:\sync.com\Sync\LYM-sources\RecordReplay\RecordOnlyOneScene.pl .\RecordReplayData\OSCreplay\replayFullPerformance.txt 4 .\RecordReplayData\OSCreplay\scene4.txt
perl D:\sync.com\Sync\LYM-sources\RecordReplay\MergeScenes.pl sequence 4 .\RecordReplayData\OSCreplay\scene1.txt .\RecordReplayData\OSCreplay\scene2.txt .\RecordReplayData\OSCreplay\scene3.txt .\RecordReplayData\OSCreplay\scene4.txt .\RecordReplayData\OSCreplay\sceneAll.txt
REM perl D:\sync.com\Sync\LYM-sources\RecordReplay\MergeScenes.pl parallel 4 .\RecordReplayData\OSCreplay\scene1.txt .\RecordReplayData\OSCreplay\scene2.txt .\RecordReplayData\OSCreplay\scene3.txt .\RecordReplayData\OSCreplay\scene4.txt .\RecordReplayData\OSCreplay\sceneMerged.txt

cd D:\sync.com\Sync\LYM-projects\batFiles\