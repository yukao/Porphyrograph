Sub HighlightScenario()

'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene

'variables
Dim i As Integer, j As Integer, lineNo As Integer, nbIntDiff As Integer, nbExtDiff As Integer, nbScenes As Integer, nbVariables As Integer
Dim extDiffInitial As Boolean, intDiff As Boolean, extDiffFinal As Boolean

' number of scnes and variables
  nbScenes = ActiveSheet.Cells(12, 2).Value
  If nbScenes <= 0 Then
     MsgBox "No scene in current scenario"
     Exit Sub
  End If
  nbVariables = ActiveSheet.Cells(18, Columns.Count).End(xlToLeft).Column - 1
  MsgBox nbScenes & " scenes " & nbVariables & " variables"

' column width autofit for whole worksheet
  ActiveSheet.Columns.AutoFit

' values aligned left for whole worksheet
  ActiveSheet.Columns.HorizontalAlignment = xlLeft
  For i = 0 To nbScenes - 1
     ' interpolations centered and line border under
     lineNo = 18 + i * 7
     ActiveSheet.Rows(lineNo).HorizontalAlignment = xlCenter
     ActiveSheet.Rows(lineNo).Borders(xlEdgeBottom).LineStyle = xlContinuous
     ' bold font for scene name
     lineNo = 13 + i * 7
     ActiveSheet.Rows(lineNo).Font.Bold = True
     ActiveSheet.Rows(lineNo).Borders(xlEdgeTop).LineStyle = xlContinuous
  Next i
  ActiveSheet.Rows(4).Borders(xlEdgeBottom).LineStyle = xlContinuous

'Loop on all the scenario values
  nbIntDiff = 0
  nbExtDiff = 0
  ' cleaning up possible previous highlight
  For j = 1 To (nbVariables + 1)
     ActiveSheet.Cells(4, j).Interior.ColorIndex = xlColorIndexNone
     ActiveSheet.Cells(4, j).Font.Bold = False
  Next j
  For i = 0 To nbScenes - 1
     lineNo = 16 + i * 7
     For j = 1 To (nbVariables + 1)
         ActiveSheet.Cells(lineNo, j).Interior.ColorIndex = xlColorIndexNone
         ActiveSheet.Cells(lineNo + 1, j).Interior.ColorIndex = xlColorIndexNone
         ActiveSheet.Cells(lineNo + 2, j).Interior.ColorIndex = xlColorIndexNone
         ActiveSheet.Cells(lineNo + 2, j).Font.Bold = False
     Next j
  Next i

  ' calculating new highlights and counting them
  For i = 0 To nbScenes - 1
     lineNo = 16 + i * 7
     For j = 1 To (nbVariables + 1)
         ' diff of initial value of current scene with end value of preceding scene
         extDiffInitial = False
         If i > 0 Then
            extDiffInitial = ActiveSheet.Cells(lineNo, j).Value <> ActiveSheet.Cells(lineNo - 6, j).Value
         End If
         
         ' diff of initial value of current scene with end value of current scene
         intDiff = ActiveSheet.Cells(lineNo, j).Value <> ActiveSheet.Cells(lineNo + 1, j).Value
         ' count
         If intDiff Then
            nbIntDiff = nbIntDiff + 1
            ActiveSheet.Cells(4, j).Interior.Color = RGB(220, 220, 220)
            ActiveSheet.Cells(4, j).Font.Bold = True
         End If
         
         ' diff of end value of current scene with initial value of next scene
         extDiffFinal = False
         If i < nbScenes - 1 Then
            extDiffFinal = ActiveSheet.Cells(lineNo + 1, j).Value <> ActiveSheet.Cells(lineNo + 7, j).Value
         End If
         ' count
         If extDiffFinal Then
            nbExtDiff = nbExtDiff + 1
            ActiveSheet.Cells(4, j).Interior.Color = RGB(220, 220, 220)
            ActiveSheet.Cells(4, j).Font.Bold = True
         End If
         
         ' coloring of initial value cell
         If extDiffInitial And intDiff Then
            ActiveSheet.Cells(lineNo, j).Interior.Color = RGB(255, 0, 255)
         Else
            If extDiffInitial Then
               ActiveSheet.Cells(lineNo, j).Interior.Color = RGB(128, 128, 255)
            Else
                If intDiff Then
                   ActiveSheet.Cells(lineNo, j).Interior.Color = RGB(255, 128, 128)
                End If
            End If
         End If
     
         ' coloring of final value cell
         If extDiffFinal And intDiff Then
            ActiveSheet.Cells(lineNo + 1, j).Interior.Color = RGB(255, 0, 255)
         Else
            If extDiffFinal Then
               ActiveSheet.Cells(lineNo + 1, j).Interior.Color = RGB(128, 128, 255)
            Else
                If intDiff Then
                   ActiveSheet.Cells(lineNo + 1, j).Interior.Color = RGB(255, 128, 128)
                End If
            End If
         End If
         
         ' highlighting variables with internal variation and s or k cell interpolation
         If intDiff And (ActiveSheet.Cells(lineNo + 2, j).Value = "s" Or ActiveSheet.Cells(lineNo + 2, j).Value = "k") Then
            ActiveSheet.Cells(lineNo + 2, j).Interior.Color = RGB(255, 0, 0)
            ActiveSheet.Cells(lineNo + 2, j).Font.Bold = True
         End If
         ' highlighting variables with external variation and k cell interpolation
         If extDiffInitial And ActiveSheet.Cells(lineNo + 2, j).Value = "k" Then
            ActiveSheet.Cells(lineNo + 2, j).Interior.Color = RGB(255, 0, 0)
            ActiveSheet.Cells(lineNo + 2, j).Font.Bold = True
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
Dim rng As Range
Set rng = Selection
Dim cell As Range
Dim equal As Boolean

For Each cell In rng.Rows(1).Cells
  equal = True
  For i = 1 To rng.Rows.Count - 1
    If Not (cell.Value = cell.Offset(i, 0).Value) Then
       equal = False
    End If
  Next i
  If equal Then
    cell.Font.Color = RGB(0, 220, 0)
    cell.Font.Bold = False
    For i = 1 To rng.Rows.Count - 1
            cell.Offset(i, 0).Font.Color = RGB(0, 220, 0)
            cell.Offset(i, 0).Font.Bold = False
    Next i
  Else
    cell.Font.Color = RGB(220, 0, 0)
    cell.Font.Bold = True
    For i = 1 To rng.Rows.Count - 1
      If Not (cell.Value = cell.Offset(i, 0).Value) Then
            cell.Offset(i, 0).Font.Color = RGB(220, 0, 0)
            cell.Offset(i, 0).Font.Bold = True
      Else
            cell.Offset(i, 0).Font.Color = RGB(0, 220, 0)
            cell.Offset(i, 0).Font.Bold = False
      End If
    Next i
  End If
Next cell

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
Dim rng As Range
    
'Loop on all the scenario values
  nbScenes = ActiveSheet.Cells(12, 2).Value
  If nbScenes <= 0 Then
     MsgBox "No scene in current scenario"
     Exit Sub
  End If
  nbVariables = ActiveSheet.Cells(18, Columns.Count).End(xlToLeft).Column - 1
  
  ' hiding/unhiding the non modified variables
  If ActiveSheet.Columns(1).Hidden Then
    For j = 1 To (nbVariables + 1)
       ActiveSheet.Columns(j).Hidden = False
    Next j
    ' unhiding header lines
    For j = 5 To 11
        ActiveSheet.Rows(j).Hidden = False
    Next j
    ' unhiding scene and variable names
    For i = 0 To nbScenes - 1
        lineNo = 14 + i * 7
        ActiveSheet.Rows(lineNo).Hidden = False
        ActiveSheet.Rows(lineNo + 1).Hidden = False
        ActiveSheet.Rows(lineNo + 5).Hidden = False
    Next i
  Else
    For j = 1 To (nbVariables + 1)
       If ActiveSheet.Cells(4, j).Interior.ColorIndex <> xlColorIndexNone Then
          ActiveSheet.Columns(j).Hidden = False
       Else
          ActiveSheet.Columns(j).Hidden = True
       End If
    Next j
    ' hiding header lines
    For j = 5 To 11
        ActiveSheet.Rows(j).Hidden = True
    Next j
    ' hiding scene and variable names + blank line
    For i = 0 To nbScenes - 1
        lineNo = 14 + i * 7
        ActiveSheet.Rows(lineNo).Hidden = True
        ActiveSheet.Rows(lineNo + 1).Hidden = True
        ActiveSheet.Rows(lineNo + 5).Hidden = True
    Next i
  End If
End Sub

