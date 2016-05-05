from distutils.core import setup
setup(name='bcrspendfrom',
      version='1.0',
      description='Command-line utility for bitcredit "coin control"',
      author='Gavin Andresen',
      author_email='gavin@bitcreditfoundation.org',
      requires=['jsonrpc'],
      scripts=['spendfrom.py'],
      )
