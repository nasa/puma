# Create a textile
Textile = CTextileWeave3D(8, 4, 5, 7);

# Add some layers of yarns
Textile.AddYLayers(0, 1);
Textile.AddYLayers(2, 1);
Textile.AddYLayers(4, 1);
Textile.AddYLayers(6, 1);
Textile.AddXLayers();
Textile.AddYLayers();
Textile.AddXLayers();
Textile.AddYLayers();
Textile.AddXLayers();
Textile.AddYLayers();

# Set the weave pattern
Textile.PushUp(0, 0);
Textile.PushUp(1, 0);
Textile.PushDown(4, 0);
Textile.PushUp(7, 0);

Textile.PushUp(1, 1);
Textile.PushUp(2, 1);
Textile.PushUp(3, 1);
Textile.PushDown(6, 1);

Textile.PushDown(0, 2);
Textile.PushUp(3, 2);
Textile.PushUp(4, 2);
Textile.PushUp(5, 2);

Textile.PushDown(2, 3);
Textile.PushUp(5, 3);
Textile.PushUp(6, 3);
Textile.PushUp(7, 3);

# Set the yarn widths and heights
Textile.SetYarnWidths(4);
Textile.SetYarnHeights(1);

# Assign the default domain
Textile.AssignDefaultDomain()

# Add the textile
AddTextile(Textile)

