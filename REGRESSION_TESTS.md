# Regression tests

Run these tests before release to make sure known bugs don't reappear.


## Gramophony parameters getting lost (issue #1)

* Load all Plugins that are going to be released to a DAW (EG. Ableton Live).
* Change all knobs from default and save tha project.
* Close the DAW and reopen it
* Check that no parameter is reset to default values.

## Check if plugin is CPU heavy

* Start several instances of the plugin that is going to be released to a DAW.
* Make sure the CPU usage is not significally higher than for other plugins.
