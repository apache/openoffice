Dim gDoc As Object

Function Gr(c1 As Long, c2 As Long, b As Integer)
  Dim g As New com.sun.star.awt.Gradient
  g.Style = com.sun.star.awt.GradientStyle.LINEAR : g.StartColor = c1 : g.EndColor = c2
  g.Angle = 0 : g.Border = b : g.XOffset = 50 : g.YOffset = 50
  g.StartIntensity = 100 : g.EndIntensity = 100 : g.StepCount = 0
  Gr = g
End Function

Sub Fill(o As Object, pre As String, kind As String, png As String)
  Dim h As New com.sun.star.drawing.Hatch
  Select Case kind
  Case "solid"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.SOLID)
    o.setPropertyValue(pre + "FillColor", &H33AA55)
  Case "solid_transp"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.SOLID)
    o.setPropertyValue(pre + "FillColor", &H3355AA)
    o.setPropertyValue(pre + "FillTransparence", 40)
  Case "gradient", "shared"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.GRADIENT)
    o.setPropertyValue(pre + "FillGradient", Gr(&HFF0000, &H0000FF, 10))
  Case "gradient_transp"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.GRADIENT)
    o.setPropertyValue(pre + "FillGradient", Gr(&HFF0000, &H0000FF, 10))
    o.setPropertyValue(pre + "FillTransparence", 30)
  Case "gradient_tgrad"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.GRADIENT)
    o.setPropertyValue(pre + "FillGradient", Gr(65280, &HFF00FF, 0))
    Dim tg As Object
    tg = gDoc.createInstance("com.sun.star.drawing.TransparencyGradientTable")
    If Not tg.hasByName("TG1") Then tg.insertByName("TG1", Gr(0, &HFFFFFF, 0))
    o.setPropertyValue(pre + "FillTransparenceGradientName", "TG1")
  Case "hatch", "hatch_nobg"
    h.Style = com.sun.star.drawing.HatchStyle.SINGLE : h.Color = 43520 : h.Distance = 200 : h.Angle = 450
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.HATCH)
    o.setPropertyValue(pre + "FillHatch", h)
    If kind = "hatch" Then
      o.setPropertyValue(pre + "FillColor", &HFFFFCC)
      o.setPropertyValue(pre + "FillBackground", True)
    Else
      o.setPropertyValue(pre + "FillBackground", False)
    End If
  Case "bitmap_tile", "bitmap_stretch", "bitmap_once"
    o.setPropertyValue(pre + "FillStyle", com.sun.star.drawing.FillStyle.BITMAP)
    o.setPropertyValue(pre + "FillBitmapURL", png)
    If kind = "bitmap_stretch" Then
      o.setPropertyValue(pre + "FillBitmapMode", com.sun.star.drawing.BitmapMode.STRETCH)
    ElseIf kind = "bitmap_once" Then
      o.setPropertyValue(pre + "FillBitmapMode", com.sun.star.drawing.BitmapMode.NO_REPEAT)
    Else
      o.setPropertyValue(pre + "FillBitmapMode", com.sun.star.drawing.BitmapMode.REPEAT)
    End If
  End Select
End Sub

Sub Gen
  Dim out As String, png As String
  out = Environ("PFF_OUT") : png = Environ("PFF_PNG")
  Dim a(0) As New com.sun.star.beans.PropertyValue
  a(0).Name = "Hidden" : a(0).Value = True
  Dim doc As Object
  doc = StarDesktop.loadComponentFromURL("private:factory/swriter", "_blank", 0, a())
  gDoc = doc
  Dim kinds
  kinds = Split(Environ("PFF_KINDS"), ",")
  Dim styles As Object, s As Object, i As Integer, nm As String
  styles = doc.StyleFamilies.getByName("PageStyles")
  For i = 0 To UBound(kinds)
    nm = "P_" + kinds(i)
    s = doc.createInstance("com.sun.star.style.PageStyle")
    styles.insertByName(nm, s)
    s = styles.getByName(nm)
    If kinds(i) <> "none" Then Fill s, "", kinds(i), png
  Next
  styles.getByName("P_shared").setPropertyValue("FollowStyle", "P_shared")
  s = doc.createInstance("com.sun.star.style.PageStyle")
  styles.insertByName("P_shared2", s)
  Fill styles.getByName("P_shared2"), "", "shared", png
  s = styles.getByName("P_gradient")
  s.HeaderIsOn = True
  Fill s, "Header", "hatch", png
  Fill styles.getByName("Standard"), "", "solid", png
  Fill styles.getByName("First Page"), "", "hatch_nobg", png
  Dim cur As Object, en As Object, p As Object
  cur = doc.Text.createTextCursor()
  doc.Text.insertString(cur, "first", False)
  doc.Text.insertControlCharacter(cur, com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK, False)
  For i = 0 To UBound(kinds)
    doc.Text.insertString(cur, "page P_" + kinds(i), False)
    doc.Text.insertControlCharacter(cur, com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK, False)
  Next
  doc.Text.insertString(cur, "page P_shared2", False)
  en = doc.Text.createEnumeration()
  p = en.nextElement()
  p.PageDescName = "First Page"
  For i = 0 To UBound(kinds)
    p = en.nextElement()
    p.PageDescName = "P_" + kinds(i)
  Next
  p = en.nextElement()
  p.PageDescName = "P_shared2"
  Dim b(0) As New com.sun.star.beans.PropertyValue
  b(0).Name = "FilterName" : b(0).Value = "writer8"
  doc.storeToURL(ConvertToURL(out), b())
  doc.close(True)
End Sub
