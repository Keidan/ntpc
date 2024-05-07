#!/usr/bin/env python3
import argparse, sys, os, subprocess

NTP_SERVER = "ntp.accelance.net"
MAX_CHAR_RESULT = 25

class Result:
  def __init__(self):
    self.num = 0
    self.passed = 0
    self.failed = 0
  
  def print(self, error, label):
    if error:
      self.error(label)
    else:
      self.success(label)

  def success(self, label):
    self.num = self.num + 1
    print(f'Test \033[1m{self.num:02d}\033[0m \033[38;5;208m{label}\033[0m \033[32mOK\033[0m')
    self.passed = self.passed + 1
        
  def error(self, label):
    self.num = self.num + 1
    print(f'Test \033[1m{self.num:02d}\033[0m \033[38;5;208m{label}\033[0m \033[31mFAILED\033[0m')
    self.failed = self.failed + 1

  def get_count(self) -> int:
    return self.num
  
  def get_passed(self) -> int:
    return self.passed
  
  def get_failed(self) -> int:
    return self.failed

class TU:
  def __init__(self, file):
    self.file = file
    self.res = Result()

  
  def format(self, label) -> str:
    return f'{label:{MAX_CHAR_RESULT}s}'
    
  def exec_process(self, args, use_std = True) -> int:
    if use_std:
      process = subprocess.Popen(args)
      _, _ = process.communicate()
    else:
      process = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
      process.communicate()
    exit_code = process.wait()
    return exit_code

  def generic(self):
    label = self.format('Opt error')
    ret_code = self.exec_process([self.file, '-0'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Help')
    ret_code = self.exec_process([self.file, '-h'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('Version')
    ret_code = self.exec_process([self.file, '-v'], False)
    self.res.print(ret_code != 0, label)

    label = self.format('Address error 1')
    ret_code = self.exec_process([self.file], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Address error 2')
    ret_code = self.exec_process([self.file, '-a', '98fgjk'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Port error 1')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '-p', '0'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Port error 2')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '-p', '65536'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Port error 3')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '-p', 'azerty'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Count error 1')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '--count', 'azer'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Count error 1')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '--count', 'azer'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Count error 2')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '--count', 'azer', '--continue'], False)
    self.res.print(ret_code == 0, label)

    label = self.format('Single')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER], False)
    self.res.print(ret_code != 0, label)

    label = self.format('Count')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '--count', '1'], False)
    self.res.print(ret_code != 0, label)

    # Should fail because github actions don't have the necessary rights to change date and time
    label = self.format('Update')
    ret_code = self.exec_process([self.file, '-a', NTP_SERVER, '-u'], False)
    self.res.print(ret_code == 0, label)

  def print_end(self):
    print(f'Total of tests \033[1m{self.res.get_count():02d}\033[0m')
    print(f'- \033[31mFAILED\033[0m \033[1m{self.res.get_failed():2d}\033[0m')

def main() -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument('-f', '--file', help='Binary file.')
  args = parser.parse_args()
  if args.file == None:
    print('Unspecified file name')
    return 1
  if not os.path.exists(args.file):
    print('File {0} not found'.format(args.file))
    return 1
    
  tu = TU(args.file)
  tu.generic()
  tu.print_end()
  return 0
  
if __name__ == '__main__':
  sys.exit(main())
