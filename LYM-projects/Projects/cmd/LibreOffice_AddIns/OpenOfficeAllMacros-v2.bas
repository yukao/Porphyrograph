Sub HighlightScenario()

'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene

'variables
Dim i As Integer, j As Integer, lineNo As Integer, nbIntDiff As Integer, nbExtDiff As Integer, nbScenes As Integer, nbVariables As Integer
Dim extDiffInitial As Boolean, intDiff As Boolean, extDiffFinal As Boolean

' number of scnes and variables
  nbScenes = ThisComponent.getCurrentController().getActiveSheet().getCellRangeByName("B12").Value
  If nbScenes <= 0 Then
     MsgBox nbScenes & "No scene in current scenario"
     Exit Sub
  End If
  oSheet = ThisComponent.Sheets.getByIndex(0)
  oRow = oSheet.Rows.getByIndex(0)
  oBlanks = oRow.queryEmptyCells()
  nbVariables = oBlanks.getByIndex(0).RangeAddress.StartColumn - 1
  MsgBox nbScenes & " scenes " & nbVariables & " variables"

' column width autofit for whole worksheet
  oColumns =  ThisComponent.getCurrentController().getActiveSheet().Columns
  For i = oColumns.getCount() - 1 To 0 Step -1
		oColumn = oColumns.getByIndex(i)
		oColumn.OptimalWidth = True
  Next i
		
' values aligned left for whole worksheet
  Dim lineFormat as New com.sun.star.table.BorderLine2
  Dim cStyle as Object
  Set cStyle = com.sun.star.table.BorderLineStyle
  For i = 0 To nbScenes - 1
     ' interpolations centered and line border under
     lineNo = 18 + i * 7
     lineFormat.LineStyle = cStyle.SOLID
     lineFormat.LineWidth = 20
     For j = 1 To (nbVariables + 1)
         oCell = ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo)
         oCell.CellBackColor = RGB(255, 255, 255)
    	 oCell.TopBorder = lineFormat
     Next j
      
     ' bold font for scene name
     lineNo = 13 + i * 7
     ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo).CharWeight  = 200
     ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo).TopBorder = lineFormat
  Next i
  ThisComponent.getCurrentController().getActiveSheet().Rows(4).BottomBorder = lineFormat

'Loop on all the scenario values
  nbIntDiff = 0
  nbExtDiff = 0
  ' cleaning up possible previous highlight
  For j = 1 To (nbVariables + 1)
     ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CellBackColor = RGB(255, 255, 255)
     ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CharWeight  = 100
  Next j
  For i = 0 To nbScenes - 1
     lineNo = 16 + i * 7
     For j = 1 To (nbVariables + 1)
         ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).CellBackColor = RGB(255, 255, 255)
         ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).CellBackColor = RGB(255, 255, 255)
         ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CellBackColor = RGB(255, 255, 255)
         ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CharWeight  = 100
     Next j
  Next i

  ' calculating new highlights and counting them
  For i = 0 To nbScenes - 1
     lineNo = 16 + i * 7
     For j = 1 To (nbVariables + 1)
         ' diff of initial value of current scene with end value of preceding scene
         extDiffInitial = False
         If i > 0 Then
            extDiffInitial = ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).String <> ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo - 6).String
         End If
         
         ' diff of initial value of current scene with end value of current scene
         intDiff = ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).String <> ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).String
        
         ' count
         If intDiff Then
            nbIntDiff = nbIntDiff + 1
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CellBackColor = RGB(220, 220, 220)
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CharWeight  = 200
         End If
         
         ' diff of end value of current scene with initial value of next scene
         extDiffFinal = False
         If i < nbScenes - 1 Then
            extDiffFinal = ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).String <> ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 7).String
         End If
         ' count
         If extDiffFinal Then
            nbExtDiff = nbExtDiff + 1
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CellBackColor = RGB(220, 220, 220)
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CharWeight  = 200
         End If
         
         ' coloring of initial value cell
         If extDiffInitial And intDiff Then
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).CellBackColor = RGB(255, 0, 255)
         Else
            If extDiffInitial Then
               ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).CellBackColor = RGB(128, 128, 255)
            Else
                If intDiff Then
                   ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo).CellBackColor = RGB(255, 128, 128)
                End If
            End If
         End If
     
         ' coloring of final value cell
         If extDiffFinal And intDiff Then
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).CellBackColor = RGB(255, 0, 255)
         Else
            If extDiffFinal Then
               ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).CellBackColor = RGB(128, 128, 255)
            Else
                If intDiff Then
                   ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 1).CellBackColor = RGB(255, 128, 128)
                End If
            End If
         End If
         
         ' highlighting variables with internal variation and s or k cell interpolation
         If intDiff And (ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).String = "s" Or ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).String = "k") Then
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CellBackColor = RGB(255, 0, 0)
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CharWeight  = 200
         End If
         ' highlighting variables with external variation and k cell interpolation
         If extDiffInitial And ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).String = "k" Then
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CellBackColor = RGB(255, 0, 0)
            ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, lineNo + 2).CharWeight  = 200
         End If
     Next j
  Next i

'Report Out Message
  MsgBox "Discontinuities Internal: " & nbIntDiff & " / External: " & nbExtDiff

End Sub



Sub ComparesRange()

'PURPOSE: Compares cells between selected rows

'variables
Dim i As Integer
Dim equal As Boolean

rng = ThisComponent.CurrentSelection
address = rng.RangeAddress
For j = 0 To rng.Columns.getCount() - 1
  Set cell = rng.getCellByPosition( j, 0 )
  equal = True
  Set cell2 = rng.getCellByPosition( j, 1 )
  For i = 1 To rng.Rows.getCount() - 1
  	Set cell3 = rng.getCellByPosition( j, i )
    If Not (cell.String = cell3.String) Then
       equal = False
    End If
  Next i
  If equal Then
    cell.CharColor = RGB(0, 220, 0)
    cell.CharWeight  = 100
    For i = 1 To rng.Rows.Count - 1
            cell2.CharColor = RGB(0, 220, 0)
            cell2.CharWeight  = 100
    Next i
    ThisComponent.getCurrentController().getActiveSheet().Columns(j + address.StartColumn).isVisible = False
  Else
    cell.CharColor = RGB(220, 0, 0)
    cell.CharWeight  = 200
    For i = 1 To rng.Rows.Count - 1
  	  Set cell3 = rng.getCellByPosition( j, i )
  	  If Not (cell.String = cell3.String) Then
            cell3.CharColor = RGB(220, 0, 0)
            cell3.CharWeight  = 200
      Else
            cell3.CharColor = RGB(0, 220, 0)
            cell3.CharWeight  = 100
      End If
    Next i
    ThisComponent.getCurrentController().getActiveSheet().Columns(j + address.StartColumn).isVisible = True
  End If
Next j

 'Will return address of selected range
'MsgBox rng.Address

'will return row num
'MsgBox rng.Row

'will give start row
'MsgBox "Start Row : " & rng.Row

'will give end row
'MsgBox "End Row : " & rng.Row + rng.Rows.Count - 1

End Sub


Sub ShowHideUnmodifiedValues()

'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene

'variables
Dim i As Integer, j As Integer, lineNo As Integer, nbIntDiff As Integer, nbExtDiff As Integer, nbScenes As Integer, nbVariables As Integer
Dim extDiffInitial As Boolean, intDiff As Boolean, extDiffFinal As Boolean
    
'Loop on all the scenario values
  nbScenes = ThisComponent.getCurrentController().getActiveSheet().getCellRangeByName("B12").Value
  If nbScenes <= 0 Then
     MsgBox "No scene in current scenario"
     Exit Sub
  End If
  oSheet = ThisComponent.Sheets.getByIndex(0)
  oRow = oSheet.Rows.getByIndex(0)
  oBlanks = oRow.queryEmptyCells()
  nbVariables = oBlanks.getByIndex(0).RangeAddress.StartColumn - 1
  MsgBox nbScenes & " scenes " & nbVariables & " variables"

  
  ' hiding/unhiding the non modified variables
  If ThisComponent.getCurrentController().getActiveSheet().Columns(1).isVisible = False Then
    For j = 1 To (nbVariables + 1)
       ThisComponent.getCurrentController().getActiveSheet().Columns(j).isVisible = True
    Next j
    ' unhiding header lines
    For j = 5 To 11
        ThisComponent.getCurrentController().getActiveSheet().Rows(j).isVisible = True
    Next j
    ' unhiding scene and variable names
    For i = 0 To nbScenes - 1
        lineNo = 14 + i * 7
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo).isVisible = True
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo + 1).isVisible = True
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo + 5).isVisible = True
    Next i
  Else
    For j = 1 To (nbVariables + 1)
       If ThisComponent.getCurrentController().getActiveSheet().getCellByPosition(j, 4).CellBackColor <> RGB(255, 255, 255) Then
          ThisComponent.getCurrentController().getActiveSheet().Columns(j).isVisible = True
       Else
          ThisComponent.getCurrentController().getActiveSheet().Columns(j).isVisible = False
       End If
    Next j
    ' hiding header lines
    For j = 5 To 11
        ThisComponent.getCurrentController().getActiveSheet().Rows(j).isVisible = False
    Next j
    ' hiding scene and variable names + blank line
    For i = 0 To nbScenes - 1
        lineNo = 14 + i * 7
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo).isVisible = False
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo + 1).isVisible = False
        ThisComponent.getCurrentController().getActiveSheet().Rows(lineNo + 5).isVisible = False
    Next i
  End If
End Sub


