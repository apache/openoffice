//! Lapce volt: launches uno_lsp.py so UNO navigation works beside clangd.
//!
//! A Lapce plugin is not a language server -- it is a WASI shim whose only job
//! is to tell the editor what command to run.  All the intelligence lives in
//! build/tools/uno_lsp.py; this file exists because Lapce has no way to declare
//! a language server in settings.toml.  The `[<volt>.volt] serverPath` setting
//! only overrides the binary of an already-installed plugin, so adding a second
//! server for C++ requires a second plugin.
//!
//! Build:
//!     cargo build --release --target wasm32-wasip1
//!     copy target/wasm32-wasip1/release/lapce_uno.wasm  -> lapce-uno.wasm
//!     copy that plus volt.toml into
//!       %LOCALAPPDATA%/lapce/Lapce-Stable/data/plugins/lapce-uno/

use lapce_plugin::{
    psp_types::{
        lsp_types::{request::Initialize, DocumentFilter, DocumentSelector, InitializeParams, Url},
        Request,
    },
    register_plugin, LapcePlugin, PLUGIN_RPC,
};
use serde_json::Value;

#[derive(Default)]
struct State {}

register_plugin!(State);

/// Read one `volt.<key>` setting.
///
/// Lapce's own volts declare these as `[config."volt.serverPath"]`, but how the
/// key arrives in initialization_options is not something the plugin API
/// promises, so both the flat and the nested spelling are accepted rather than
/// betting on one and failing silently at startup.
fn setting(options: Option<&Value>, key: &str) -> Option<String> {
    let opts = options?;
    let flat = opts.get(format!("volt.{key}"));
    let nested = opts.get("volt").and_then(|v| v.get(key));
    flat.or(nested)
        .and_then(|v| v.as_str())
        .map(str::to_string)
        .filter(|s| !s.is_empty())
}

fn start(params: InitializeParams) -> anyhow::Result<()> {
    let options = params.initialization_options.as_ref();

    let workspace = setting(options, "workspace")
        .or_else(|| {
            // Fall back to the folder Lapce opened, which is the checkout in
            // the normal case and saves configuring anything at all.
            params
                .workspace_folders
                .as_ref()
                .and_then(|f| f.first())
                .and_then(|f| f.uri.to_file_path().ok())
                .map(|p| p.to_string_lossy().replace('\\', "/"))
        })
        .ok_or_else(|| anyhow::anyhow!("uno: volt.workspace is not set"))?;

    let db = setting(options, "dbPath").unwrap_or_else(|| format!("{workspace}/uno.sqlite"));
    let python = setting(options, "serverPath").unwrap_or_else(|| "python".to_string());

    // A bare name is resolved from PATH via the urn: form; anything that looks
    // like a path is passed as a file URL.
    let server_uri = if python.contains('/') || python.contains('\\') {
        Url::parse(&format!("file:///{}", python.replace('\\', "/").trim_start_matches('/')))?
    } else {
        Url::parse(&format!("urn:{python}"))?
    };

    let args = vec![
        format!("{workspace}/build/tools/uno_lsp.py"),
        "--db".to_string(),
        db,
        "--workspace".to_string(),
        workspace,
    ];

    let selector: DocumentSelector = ["c", "cpp"]
        .iter()
        .map(|lang| DocumentFilter {
            language: Some((*lang).to_string()),
            pattern: None,
            scheme: None,
        })
        .collect();

    // options is deliberately None: uno_lsp takes its configuration from argv,
    // and forwarding Lapce's volt settings would only hand the server keys it
    // does not understand.
    PLUGIN_RPC.start_lsp(server_uri, args, selector, None);
    Ok(())
}

impl LapcePlugin for State {
    fn handle_request(&mut self, _id: u64, method: String, params: Value) {
        if method.as_str() == Initialize::METHOD {
            match serde_json::from_value::<InitializeParams>(params) {
                Ok(params) => {
                    if let Err(e) = start(params) {
                        PLUGIN_RPC.stderr(&format!("lapce-uno: {e}"));
                    }
                }
                Err(e) => PLUGIN_RPC.stderr(&format!("lapce-uno: bad InitializeParams: {e}")),
            }
        }
    }
}
