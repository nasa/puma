from TexGen.Abaqus import *

shear = CLinearTransformation()
shear.AddSheardYbydX(0.1)
shear.AddSheardXbydY(0.1)

tension = CLinearTransformation()
tension.AddScale(1.1)

deformer = TextileDeformerAbaqus()
deformer.AddDeformationStep(shear)
deformer.AddDeformationStep(shear*tension)
#deformer.SetFortranProgram('LinearElasticUMAT.for')
#deformer.SetMaterial("ISO_", [1.0, 0.0])

ReadFromXML('pweave.tg3')
textile = GetTextile()

deformer.DeformTextile(textile)

SaveToXML('pweave-deformed.tg3')
