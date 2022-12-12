'  *****  BASIC  *****

Sub Main

End Sub

' -------------------------------------------------------------------------------------------------------
'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene
' -------------------------------------------------------------------------------------------------------
Sub HighlightScenario

' -------------------------------------------------------------
' define variables
dim document as object
dim ActiveSheet as object
dim ActiveCell as object
dim ActiveColumns as object
Dim i As Integer
Dim j As Integer
Dim lineNo As Integer
Dim nbIntDiff As Integer
Dim nbExtDiff As Integer
Dim nbScenes As Integer
Dim nbVariables As Integer
Dim col As Integer
Dim extDiffInitial As Boolean
Dim intDiff As Boolean
Dim extDiffFinal As Boolean
Dim variableName As object
    
' -------------------------------------------------------------
' get access to the document
document = ThisComponent
ActiveSheet = document.Sheets(0)

' number of scnes and variables
' when compared with Excel: exchange lines and columns and subtract 1 for each dimension
ActiveCell = ActiveSheet.getCellByPosition(1, 11)
nbScenes = ActiveCell.Value
If nbScenes <= 0 Then
   MsgBox "No scene in current scenario"
   Exit Sub
End If

ActiveColumns = ActiveSheet.Columns
For col = 0 To ActiveColumns.getCount() - 1
   variableName = ActiveSheet.getCellByPosition(col, 3)
   If Len(variableName.String) > 0 Then
   		nbVariables = col
   Else
        Exit For
   End If
Next

MsgBox nbScenes & " scenes " & nbVariables & " variables"

' cleaning up possible previous highlight
' left justification, white background, normal font weight for whole worksheet
ActiveSheet.setPropertyValue("HoriJustify", com.sun.star.table.CellHoriJustify.LEFT)
ActiveSheet.setPropertyValue("CellBackColor", rgb(255,255,255) )
ActiveSheet.setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.NORMAL )   

Dim borderLine As New com.sun.star.table.BorderLine2
With borderLine
    .LineStyle = com.sun.star.table.BorderLineStyle.SOLID 
    .OuterLineWidth = 5
End With
For i = 0 To nbScenes - 1
   ' interpolations centered and line border under
   lineNo = 18 + i * 7 - 1
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("HoriJustify", com.sun.star.table.CellHoriJustify.CENTER)
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
   ' bold font for scene name
   lineNo = 13 + i * 7 - 1
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
   ' background color for scene names line to help orient horizontally
   lineNo = 14 + i * 7 - 1
	Select Case (i mod 6)    ' Evaluate Number. 
	Case 0
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,235,255)) 
	Case 1
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,235,235)) 
	Case 2
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,255,235)) 
	Case 3
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,235,255)) 
	Case 4
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,255,235)) 
	Case 5
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,255,255)) 
	End Select   
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
    lineNo = 15 + i * 7 - 1
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
Next i
ActiveSheet.Rows.getByIndex(4 - 1).setPropertyValue("BottomBorder", borderLine)
ActiveSheet.Columns.getByIndex(0).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)

' calculating new highlights and counting them
nbIntDiff = 0
nbExtDiff = 0
For i = 0 To nbScenes - 1
   lineNo = 16 + i * 7
   For j = 1 To (nbVariables + 1)
       ' diff of initial value of current scene with end value of preceding scene
       extDiffInitial = False
       If i > 0 Then
          extDiffInitial = ActiveSheet.getCellByPosition(j - 1, lineNo - 1).Value <> ActiveSheet.getCellByPosition(j - 1, lineNo - 7).Value
       End If
       
       ' diff of initial value of current scene with end value of current scene
       intDiff = ActiveSheet.getCellByPosition(j - 1, lineNo - 1).Value <> ActiveSheet.getCellByPosition(j - 1, lineNo).Value
       ' count
       If intDiff Then
          nbIntDiff = nbIntDiff + 1
          ActiveSheet.getCellByPosition(j - 1, 3).CellBackColor = rgb(220, 220, 220)
          ActiveSheet.getCellByPosition(j - 1, 3).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
       End If
       
       ' diff of end value of current scene with initial value of next scene
       extDiffFinal = False
       If i < nbScenes - 1 Then
          extDiffFinal = ActiveSheet.getCellByPosition(j - 1, lineNo).Value <> ActiveSheet.getCellByPosition(j - 1, lineNo + 6).Value
       End If
       ' count
       If extDiffFinal Then
          nbExtDiff = nbExtDiff + 1
          ActiveSheet.getCellByPosition(j - 1, 3).CellBackColor = rgb(220, 220, 220)
          ActiveSheet.getCellByPosition(j - 1, 3).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
       End If
       
       ' coloring of initial value cell
       If extDiffInitial And intDiff Then
          ActiveSheet.getCellByPosition(j - 1, lineNo - 1).CellBackColor = rgb(255, 0, 255)
       Else
          If extDiffInitial Then
             ActiveSheet.getCellByPosition(j - 1, lineNo - 1).CellBackColor = rgb(128, 128, 255)
          Else
              If intDiff Then
                 ActiveSheet.getCellByPosition(j - 1, lineNo - 1).CellBackColor = rgb(255, 128, 128)
              End If
          End If
       End If
   
       ' coloring of final value cell
       If extDiffFinal And intDiff Then
          ActiveSheet.getCellByPosition(j - 1, lineNo).CellBackColor = rgb(255, 0, 255)
       Else
          If extDiffFinal Then
             ActiveSheet.getCellByPosition(j - 1, lineNo).CellBackColor = rgb(128, 128, 255)
          Else
              If intDiff Then
                 ActiveSheet.getCellByPosition(j - 1, lineNo).CellBackColor = rgb(255, 128, 128)
              End If
          End If
       End If
       
       ' highlighting variables with internal variation and s or k cell interpolation
       If intDiff And (ActiveSheet.getCellByPosition(j - 1, lineNo + 1).Value = "s" Or ActiveSheet.getCellByPosition(j - 1, lineNo + 1).Value = "k") Then
          ActiveSheet.getCellByPosition(j - 1, lineNo + 1).CellBackColor = rgb(255, 0, 0)
          ActiveSheet.getCellByPosition(j - 1, lineNo + 1).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
       End If
       ' highlighting variables with external variation and k cell interpolation
       If extDiffInitial And ActiveSheet.getCellByPosition(j - 1, lineNo + 1).Value = "k" Then
          ActiveSheet.getCellByPosition(j - 1, lineNo + 1).CellBackColor = rgb(255, 0, 0)
          ActiveSheet.getCellByPosition(j - 1, lineNo + 1).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
       End If
   Next j
Next i

'Report Out Message
MsgBox "Discontinuities Internal: " & nbIntDiff & " / External: " & nbExtDiff

End Sub

' -------------------------------------------------------------------------------------------------------
'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene
' -------------------------------------------------------------------------------------------------------
Sub HighlightScenarioSVG

' -------------------------------------------------------------
' define variables
dim document as object
dim ActiveSheet as object
dim ActiveCell as object
Dim SourceAddress As New com.sun.star.table.CellRangeAddress
Dim DestinationAddress As New com.sun.star.table.CellAddress
dim ActiveColumns as object
Dim i As Integer
Dim j As Integer
Dim lineNo As Integer
Dim nbScenes As Integer
Dim nbVariables As Integer
Dim col As Integer
Dim variableName As object
    
' -------------------------------------------------------------
' get access to the document
document = ThisComponent
ActiveSheet = document.Sheets(0)

' number of scnes and variables
' when compared with Excel: exchange lines and columns and subtract 1 for each dimension
ActiveCell = ActiveSheet.getCellByPosition(1, 4)
nbScenes = ActiveCell.Value
If nbScenes <= 0 Then
   MsgBox "No scene in current scenario"
   Exit Sub
End If

ActiveColumns = ActiveSheet.Columns
For col = 0 To ActiveColumns.getCount() - 1
   variableName = ActiveSheet.getCellByPosition(col, 3)
   If Len(variableName.String) > 0 Then
   		nbVariables = col
   Else
        Exit For
   End If
Next

' cleaning up possible previous highlight
' left justification, white background, normal font weight for whole worksheet
ActiveSheet.setPropertyValue("HoriJustify", com.sun.star.table.CellHoriJustify.LEFT)
ActiveSheet.setPropertyValue("CellBackColor", rgb(255,255,255) )
ActiveSheet.setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.NORMAL )   

Dim borderLine As New com.sun.star.table.BorderLine2
With borderLine
    .LineStyle = com.sun.star.table.BorderLineStyle.SOLID 
    .OuterLineWidth = 5
End With
For i = 0 To nbScenes - 1
   ' bold font for scene name
   lineNo = 6 + i * 7 - 1
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
   ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
   ' background color for scene names line to help orient horizontally
   lineNo = 7 + i * 7 - 1
   If i = 0 Or i = 9 Or i = 15 Or i = 33 Or i = 66 Or i = 69 Or i = 70 Or i = 76 Or i = 153 Or i = 155 Or i = 160 Or i = 165 Or i = 167 Then
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,255,0)) 
   Else
	Select Case (i mod 6)    ' Evaluate Number. 
	Case 0
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,235,255)) 
	Case 1
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,235,235)) 
	Case 2
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,255,235)) 
	Case 3
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,235,255)) 
	Case 4
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(255,255,235)) 
	Case 5
		ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("CellBackColor", rgb(235,255,255)) 
	End Select   
   End If
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
    'TMP extend lines with cropping values
    'SourceAddress.Sheet = 0'TMP
    'SourceAddress.StartColumn = 15'TMP
    'SourceAddress.StartRow = lineNo'TMP
    'SourceAddress.EndColumn = 20'TMP
    'SourceAddress.EndRow = lineNo'TMP
    'DestinationAddress.Sheet = 0'TMP
    'DestinationAddress.Column = 21'TMP
    'DestinationAddress.Row = lineNo'TMP
    'ActiveSheet.copyRange(DestinationAddress,SourceAddress)'TMP
    lineNo = 8 + i * 7 - 1
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
    'TMP extend lines with cropping values
    'ActiveCell = ActiveSheet.getCellByPosition(23, lineNo)'TMP
    'ActiveCell.setString("crop_offset_x")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(24, lineNo)'TMP
    'ActiveCell.setString("crop_offset_y")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(25, lineNo)'TMP
    'ActiveCell.setString("crop_size_x")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(26, lineNo)'TMP
    'ActiveCell.setString("crop_size_y")'TMP
    'TMP extend lines with cropping values -crop 3360x1890+240+135
    'lineNo = 9 + i * 7 - 1'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(23, lineNo)'TMP
    'ActiveCell.setValue(240)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(24, lineNo)'TMP
    'ActiveCell.setValue(135)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(25, lineNo)'TMP
    'ActiveCell.setValue(3360)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(26, lineNo)'TMP
    'ActiveCell.setValue(1890)'TMP
    'lineNo = 10 + i * 7 - 1'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(23, lineNo)'TMP
    'ActiveCell.setValue(240)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(24, lineNo)'TMP
    'ActiveCell.setValue(135)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(25, lineNo)'TMP
    'ActiveCell.setValue(3360)'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(26, lineNo)'TMP
    'ActiveCell.setValue(1890)'TMP
    ' interpolations centered and line border under
    lineNo = 11 + i * 7 - 1
    'TMP extend lines with cropping values -crop 3360x1890+240+135
    'ActiveCell = ActiveSheet.getCellByPosition(23, lineNo)'TMP
    'ActiveCell.setString("s")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(24, lineNo)'TMP
    'ActiveCell.setString("s")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(25, lineNo)'TMP
    'ActiveCell.setString("s")'TMP
    'ActiveCell = ActiveSheet.getCellByPosition(26, lineNo)'TMP
    'ActiveCell.setString("s")'TMP
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("HoriJustify", com.sun.star.table.CellHoriJustify.CENTER)
    ActiveSheet.Rows.getByIndex(lineNo).setPropertyValue("BottomBorder", borderLine)
Next i
ActiveSheet.Rows.getByIndex(4 - 1).setPropertyValue("BottomBorder", borderLine)
ActiveSheet.Columns.getByIndex(0).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)

'Report Out Message
MsgBox nbScenes & " scenes " & nbVariables & " variables"

End Sub

' -------------------------------------------------------------------------------------------------------
'PURPOSE: Compares cells between selected rows
' -------------------------------------------------------------------------------------------------------
Sub ComparesRange

'variables
Dim i As Integer
Dim equal As Boolean

Dim oSel as object
Dim CurrentRow as object
Dim AuxRow as object
Dim cell as object
Dim auxCell as object
Dim document as object
Dim ActiveSheet as object
Dim nbRows as Integer
Dim nbCols as Integer
Dim start_r as Integer
Dim start_c as Integer
Dim nbDifferences as Integer
Dim cell1Val as String
Dim cell2Val as String
Dim cell1 as Double
Dim cell2 as Double

' -------------------------------------------------------------
' get access to the document
document = ThisComponent
oSel = document.getCurrentSelection()
ActiveSheet = document.Sheets(0)

If oSel.supportsService("com.sun.star.sheet.SheetCellRange") Then
    nbCols = oSel.Columns.getCount()
    nbRows = oSel.Rows.getCount()
	start_r = oSel.RangeAddress.StartRow
	start_c = oSel.RangeAddress.StartColumn
	MsgBox "Single Cell Range selected, size: " & nbCols & "x" & nbRows & " from " & start_c & "x" & start_r
Else
	MsgBox "Single Cell Range not selected"
End If

nbDifferences = 0
CurrentRow = oSel.Rows.getByIndex(0)
'For Each cell in CurrentRow Cells
For k = 0 To nbCols - 1
  cell = CurrentRow.getCellByPosition(start_c + k,0)
  equal = True
  For i = 1 To nbRows - 1
  	AuxRow = oSel.Rows.getByIndex(i)
  	cell1Val = cell.getValue()
  	cell2Val = AuxRow.getCellByPosition(start_c + k,0).getValue()
  	cell1 = CDbl(cell1Val)
  	cell2 = CDbl(cell2Val)
' -- MsgBox "Values: " & cell1 & "x" & cell2 &  " values: " & cell1Val & "x" & cell2Val
    If cell1 <>  cell2 Then
       equal = False
    End If
  Next i
  If equal Then
    cell.CharColor = rgb(0, 220, 0)
    cell.setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.NORMAL)
    For i = 1 To nbRows - 1
	  	AuxRow = oSel.Rows.getByIndex(i)
        AuxRow.getCellByPosition(start_c + k,0).CharColor = rgb(0, 220, 0)
        AuxRow.getCellByPosition(start_c + k,0).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.NORMAL)
    Next i
    ActiveSheet.Columns.getByIndex(k).IsVisible = False
  Else
    nbDifferences = nbDifferences + 1
    cell.CharColor = rgb(220, 0, 0)
    cell.setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
    For i = 1 To nbRows - 1
	  AuxRow = oSel.Rows.getByIndex(i)
      If cell.Value <> AuxRow.getCellByPosition(start_c + k,0).Value Then
            AuxRow.getCellByPosition(start_c + k,0).CharColor = rgb(220, 0, 0)
            AuxRow.getCellByPosition(start_c + k,0).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
      Else
            AuxRow.getCellByPosition(start_c + k,0).CharColor = rgb(0, 220, 0)
            AuxRow.getCellByPosition(start_c + k,0).setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.NORMAL)
      End If
    Next i
    ActiveSheet.Columns.getByIndex(k).IsVisible = True
  End If
Next k

MsgBox "Found " & nbDifferences & " columns with different values inside"

End Sub

' -------------------------------------------------------------------------------------------------------
'PURPOSE: Highlight cells for which there is a change of value between two scenes or inside a scene
' -------------------------------------------------------------------------------------------------------
Sub ShowHideUnmodifiedValues

'variables
Dim i As Integer
Dim j As Integer
Dim lineNo As Integer
Dim nbIntDiff As Integer
Dim nbExtDiff As Integer
Dim nbScenes As Integer
Dim nbVariables As Integer
Dim extDiffInitial As Boolean
Dim intDiff As Boolean
Dim extDiffFinal As Boolean

Dim document As object
Dim ActiveSheet As object
Dim ActiveColumns As object
Dim variableName As object
    
' -------------------------------------------------------------
' get access to the document
document = ThisComponent
ActiveSheet = document.Sheets(0)

' number of scnes and variables
' when compared with Excel: exchange lines and columns and subtract 1 for each dimension
ActiveCell = ActiveSheet.getCellByPosition(1, 11)
nbScenes = ActiveCell.Value
If nbScenes <= 0 Then
   MsgBox "No scene in current scenario"
   Exit Sub
End If

ActiveColumns = ActiveSheet.Columns
For col = 0 To ActiveColumns.getCount() - 1
   variableName = ActiveSheet.getCellByPosition(col, 3)
   If Len(variableName.String) > 0 Then
   		nbVariables = col
   Else
        Exit For
   End If
Next


' Loop on all the scenario values
' hiding/unhiding the non modified variables
' Hides if the first column is visible
If ActiveSheet.Columns.getByIndex(0).IsVisible Then
  For j = 0 To nbVariables
     If ActiveSheet.getCellByPosition(j, 3).CellBackColor <> rgb(255,255,255) Then
        ActiveSheet.Columns.getByIndex(j).IsVisible = True
     Else
        ActiveSheet.Columns.getByIndex(j).IsVisible = False
     End If
  Next j
  ' hiding header lines
  For j = 5 To 11
      ActiveSheet.Rows.getByIndex(j).IsVisible = False
  Next j
  ' hiding scene and variable names + blank line
  For i = 0 To nbScenes - 1
      lineNo = 14 + i * 7
      ActiveSheet.Rows.getByIndex(lineNo - 1).IsVisible = False
      ActiveSheet.Rows.getByIndex(lineNo).IsVisible = False
      ActiveSheet.Rows.getByIndex(lineNo + 4).IsVisible = False
  Next i
  'Report Out Message
  MsgBox "Hiding unmodified values completed"
Else
  For j = 0 To nbVariables
     ActiveSheet.Columns.getByIndex(j).IsVisible = True
  Next j
  ' unhiding header lines
  For j = 5 To 11
      ActiveSheet.Rows.getByIndex(j).IsVisible = True
  Next j
  ' unhiding scene and variable names
  For i = 0 To nbScenes - 1
      lineNo = 14 + i * 7
      ActiveSheet.Rows.getByIndex(lineNo - 1).IsVisible = True
      ActiveSheet.Rows.getByIndex(lineNo).IsVisible = True
      ActiveSheet.Rows.getByIndex(lineNo + 4).IsVisible = True
  Next i
  MsgBox "All columns and rows visible"
End If
End Sub


