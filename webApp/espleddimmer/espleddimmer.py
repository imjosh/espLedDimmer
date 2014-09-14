#!/usr/bin/env python
#
# Copyright 2007 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import os
import urllib
import cgi
import datetime

from google.appengine.ext import ndb
from google.appengine.api import users

import jinja2
import webapp2

JINJA_ENVIRONMENT = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__)),
    extensions=['jinja2.ext.autoescape'],
    autoescape=True)

class Lamp(ndb.Model):
  status  = ndb.IntegerProperty()

myLamp = Lamp(status=10)
myLamp_key = myLamp.put()

class MainPage(webapp2.RequestHandler):
  def get(self):
    myLamp = myLamp_key.get()
    status = myLamp.status

    template_values = { 'status' : status }
    template = JINJA_ENVIRONMENT.get_template('index.html')
    self.response.write(template.render(template_values))

class Status(webapp2.RequestHandler):
  def get(self):
    myLamp = myLamp_key.get()
    self.response.out.write('status: %s' % myLamp.status)
  def post(self):
    myLamp = myLamp_key.get()
    myLamp.status = int(self.request.get('status'))
    myLamp.put()
    self.redirect('/')

app = webapp2.WSGIApplication([
  ('/', MainPage),
  ('/status', Status)
], debug=True)
