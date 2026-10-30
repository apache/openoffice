Function Gr(c1 As Long, c2 As Long, sty As Long, s As Long, e As Long)
  Dim g As New com.sun.star.awt.Gradient
  g.Style = sty : g.StartColor = c1 : g.EndColor = c2
  g.Angle = 300 : g.Border = 10 : g.XOffset = 50 : g.YOffset = 50
  g.StartIntensity = s : g.EndIntensity = e : g.StepCount = 0
  Gr = g
End Function

Sub Fill(o As Object, kind As String, png As String)
  Dim h As New com.sun.star.drawing.Hatch
  Select Case kind
  Case "solid"
    o.FillStyle = com.sun.star.drawing.FillStyle.SOLID
    o.FillColor = &H33AA55
  Case "solid_transp"
    o.FillStyle = com.sun.star.drawing.FillStyle.SOLID
    o.FillColor = &H3355AA
    o.FillTransparence = 40
  Case "gradient"
    o.FillStyle = com.sun.star.drawing.FillStyle.GRADIENT
    o.FillGradient = Gr(&HFF0000, &H0000FF, 0, 100, 100)
  Case "gradient_transp"
    o.FillStyle = com.sun.star.drawing.FillStyle.GRADIENT
    o.FillGradient = Gr(&HFF0000, &H0000FF, 0, 100, 100)
    o.FillTransparence = 30
  Case "gradient_tgrad"
    o.FillStyle = com.sun.star.drawing.FillStyle.GRADIENT
    o.FillGradient = Gr(&HFF0000, &H0000FF, 0, 100, 100)
    o.FillTransparenceGradient = Gr(&H000000, &HFFFFFF, 0, 100, 100)
  Case "hatch", "hatch_nobg"
    h.Style = com.sun.star.drawing.HatchStyle.SINGLE : h.Color = &H00AA00 : h.Distance = 200 : h.Angle = 450
    o.FillStyle = com.sun.star.drawing.FillStyle.HATCH
    o.FillHatch = h
    If kind = "hatch" Then
      o.FillColor = &HFFFFCC
      o.FillBackground = True
    Else
      o.FillBackground = False
    End If
  Case "bitmap_tile", "bitmap_stretch", "bitmap_once", "bitmap_transp"
    o.FillStyle = com.sun.star.drawing.FillStyle.BITMAP
    o.FillBitmapURL = png
    If kind = "bitmap_stretch" Then
      o.FillBitmapStretch = True
      o.FillBitmapTile = False
    ElseIf kind = "bitmap_once" Then
      o.FillBitmapStretch = False
      o.FillBitmapTile = False
    Else
      o.FillBitmapStretch = False
      o.FillBitmapTile = True
    End If
    If kind = "bitmap_transp" Then o.FillTransparence = 50
  End Select
End Sub

Sub Gen
  Dim out As String, png As String
  out = Environ("PFF_OUT") : png = Environ("PFF_PNG")
  Dim a(0) As New com.sun.star.beans.PropertyValue
  a(0).Name = "Hidden" : a(0).Value = True
  Dim doc As Object
  doc = StarDesktop.loadComponentFromURL("private:factory/swriter", "_blank", 0, a())
  Dim kinds
  kinds = Split(Environ("PFF_KINDS"), ",")
  Dim styles As Object, s As Object, i As Integer
  styles = doc.StyleFamilies.getByName("ParagraphStyles")
  For i = 0 To UBound(kinds)
    s = doc.createInstance("com.sun.star.style.ParagraphStyle")
    s.ParentStyle = "Standard"
    styles.insertByName("S_" + kinds(i), s)
    Fill s, kinds(i), png
  Next
  Dim cur As Object, k As Integer
  cur = doc.Text.createTextCursor()
  For i = 0 To UBound(kinds)
    doc.Text.insertString(cur, "direct " + kinds(i), False)
    doc.Text.insertControlCharacter(cur, com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK, False)
    doc.Text.insertString(cur, "styled " + kinds(i), False)
    doc.Text.insertControlCharacter(cur, com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK, False)
  Next
  Dim en As Object, p As Object
  en = doc.Text.createEnumeration()
  For i = 0 To UBound(kinds)
    p = en.nextElement()
    Fill p, kinds(i), png
    p = en.nextElement()
    p.ParaStyleName = "S_" + kinds(i)
  Next
  Dim b(0) As New com.sun.star.beans.PropertyValue
  b(0).Name = "FilterName" : b(0).Value = "writer8"
  doc.storeToURL(ConvertToURL(out), b())
  doc.close(True)
  StarDesktop.terminate()
End Sub
