#!/usr/bin/python2.7
# -*- coding: utf8 -*-
""" UDP listening client for the LYT8266 UDP broadcast packets"""
__author__ = 'Jan KLopper (jan@underdark.nl)'
__version__ = 0.1

import socket
import argparse
import ConfigParser


class LytScanner(object):
  def __init__(self, options):
    """This class listens for udp packets on all ethernet interfaces on the
    given port and processes them."""

    self.options = options
    self.UDPSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.UDPSock.bind(('', options.port))
    self.verbose = options.verbose

    if options.verbose:
      print 'Starting Lyt8266 listener'

  def Run(self):
    while True:
      print self.UDPSock.recvfrom(1024)[0]

  def __del__(self):
    self.UDPSock.close()


def main():
  """This program listenes to the broadcast address on the listening port and
  handles any received measurements

  Measurements are stored in:
	 sqlite (if available), logfile, Carbon, or outputs to stdout"""
  parser = argparse.ArgumentParser()
  parser.add_argument("-p", "--port", dest="port",
                    help="Listen port", default=8898)
  parser.add_argument("-v", "--verbose", dest="verbose",
                    action="store_true",
                    help="Be verbose", default=False)

  lytscanner = LytScanner(parser.parse_args())
  lytscanner.Run()

if __name__ == '__main__':
  main()
