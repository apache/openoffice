# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

PYTHON := $(shell command -v python || command -v python3 || echo python)
PIP := $(PYTHON) -m pip

.PHONY: check checkinstall checkupdate createwordlist install

check: checkinstall
	@echo "Running pre-commit checks..."
	pre-commit run --all-files

checkinstall: install
	@echo "Installing pre-commit hooks..."
	pre-commit install

checkupdate:
	@echo "Updating pre-commit hooks..."
	pre-commit autoupdate

createwordlist: install
	@echo "Regenerating the ignored words list codespell.txt"
	codespell --skip='./extras' | cut -f2 -d' ' | tr A-Z a-z | sort | uniq > .github/linters/codespell.txt

install:
	@echo "Installing dependencies..."
	@if [ -f requirements-dev.txt ]; then \
		$(PIP) install -r requirements-dev.txt; \
	else \
		echo "Error: requirements-dev.txt not found."; \
		exit 1; \
	fi
