import unittest
import FlowTexTest
import WiseTexTest
import GridFileTest
import TexGenv2Test
import AbaqusTest

if __name__ == "__main__":
    alltests = unittest.TestSuite([FlowTexTest.GetTestSuite(),
                                   WiseTexTest.GetTestSuite(),
                                   GridFileTest.GetTestSuite(),
                                   TexGenv2Test.GetTestSuite(),
                                   AbaqusTest.GetTestSuite(),
                                   ])
    unittest.TextTestRunner(verbosity=2).run(alltests)
