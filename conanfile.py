from conans import ConanFile

class ExampleUsingFlowTM4C(ConanFile):
   requires = "FlowTM4C/1.4@spiessensm/stable"

   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
