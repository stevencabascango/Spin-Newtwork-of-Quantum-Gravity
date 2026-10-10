#!/usr/bin/env bash
# Run from a regular WSL terminal outside the current Codex sandbox.
set -euo pipefail
brave_executable='/mnt/c/Program Files/BraveSoftware/Brave-Browser/Application/brave.exe'
windows_command='/mnt/c/Windows/System32/cmd.exe'
windows_powershell='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
if [[ ! -f "$brave_executable" ]]; then
    echo 'No se encontró Brave en la ruta estándar de Windows.' >&2
    exit 1
fi
if ! command -v codex >/dev/null; then
    echo 'Codex CLI no está disponible en esta terminal.' >&2
    exit 1
fi
if ! node_version=$(cd /mnt/c && "$windows_command" /d /c 'node --version'); then
    echo 'Instala Node.js LTS en Windows y abre una terminal nueva.' >&2
    echo 'Desde PowerShell: winget install --id OpenJS.NodeJS.LTS -e' >&2
    exit 1
fi
node_version=${node_version//$'\r'/}
printf 'Node.js Windows: %s\n' "$node_version"
# Parse the version in Bash, avoiding nested Windows/WSL quotes.
if [[ ! "$node_version" =~ ^v([0-9]+)\. ]] || (( BASH_REMATCH[1] < 18 )); then
    echo 'Se necesita Node.js 18 o posterior en Windows.' >&2
    exit 1
fi
if [[ ! -f "$windows_powershell" ]]; then
    echo 'No se encontró Windows PowerShell.' >&2
    exit 1
fi
# EncodedCommand transports the exact arguments across WSL and Windows.
# The payload contains only the launcher command; no credentials.
launcher=$(python3 - <<'PY'
import base64
script = r"""$ErrorActionPreference = 'Continue'
$ProgressPreference = 'SilentlyContinue'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
$OutputEncoding = [Console]::OutputEncoding
$env:NPM_CONFIG_UPDATE_NOTIFIER = 'false'
$env:PATH = 'C:\Program Files\nodejs;' + $env:PATH
Set-Location -LiteralPath $env:TEMP
& 'C:\Program Files\nodejs\node.exe' 'C:\Program Files\nodejs\node_modules\npm\bin\npx-cli.js' '-y' '@playwright/mcp@latest' '--executable-path' 'C:\Program Files\BraveSoftware\Brave-Browser\Application\brave.exe' '--isolated' '--output-dir' (Join-Path $env:TEMP 'spin-network-browser')
exit $LASTEXITCODE
"""
print(base64.b64encode(script.encode('utf-16le')).decode('ascii'))
PY
)
codex mcp add spin_brave -- "$windows_powershell" -NoLogo -NoProfile -NonInteractive -OutputFormat Text -EncodedCommand "$launcher"
codex mcp get spin_brave
printf '\nConfiguración registrada; conexión aún por comprobar.\n'
printf 'Reinicia o reanuda Codex y escribe /mcp para comprobar spin_brave.\n'
printf 'El primer inicio de MCP descargará Playwright y requiere internet.\n'
