# Diatheke SDK for Java

This project uses groovy to build a java library and publish it to GitHub's Maven Package repository.

## Setup

This assumes that you have a configured Apache Maven to use GitHub Personal Access Tokens.
See [Configuring Apache aven for use with GitHub Packages](https://docs.github.com/en/free-pro-team@latest/packages/using-github-packages-with-your-projects-ecosystem/configuring-apache-maven-for-use-with-github-packages) for more on this.

## Gradle

We use two specific gradle plugins in this project.

* `maven-publish` Allows us to push to a maven repository as a gradle task.
* `net.linguica.maven-settings` exposes Maven settings (i.e. ~/.m2/settings.xml) to Gradle projects.
  * This allows us to store our user credentials for GitHub Maven outside of this project.

For details on maven-publish, see the [Gradle user guide entry on the Maven Publishing Plugin](https://docs.gradle.org/current/userguide/publishing_maven.html)

Note: your ~/.m2/settings.xml file should look something like this:

```xml
<settings xmlns="http://maven.apache.org/SETTINGS/1.0.0"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="http://maven.apache.org/SETTINGS/1.0.0
                      http://maven.apache.org/xsd/settings-1.0.0.xsd">

  <activeProfiles>
    <activeProfile>github</activeProfile>
  </activeProfiles>

  <profiles>
    <profile>
      <id>github</id>
      <repositories>
        <repository>
          <id>central</id>
          <url>https://repo1.maven.org/maven2</url>
          <releases><enabled>true</enabled></releases>
          <snapshots><enabled>true</enabled></snapshots>
        </repository>
        <repository>
          <id>github</id>
          <name>GitHub Cobalt Speech Apache Maven Packages</name>
          <url>https://maven.pkg.github.com/cobaltspeech</url>
        </repository>
      </repositories>
    </profile>
  </profiles>

  <servers>
    <server>
      <id>github</id>
      <username>{GITHUB_USER_NAME}</username>
      <password>{GITHUB_ACCESS_TOKEN}</password>
    </server>
  </servers>
</settings>

```

### Building

To better support android, we provide a lite package along side the full version.
The only difference is that the lite is compiled with protobuf-lite options.
This helps slim down the size of the java archive.
Android uses are encuraged to use `sdk-diatheke-lite`, while other Java users can elect to use the full version.

`./gradlew :liblite:build` or `./gradlew :lib:build` can be used to make sure that the generated java code can compile.
This step should always pass without errors before publishing.

### Publishing

To publish to GitHub Packages, simply run `./gradlew publish`.
If you are building locally, you can use `./gradlew publishToMavenLocal` to publish to your `~/.m2/repositories/` at `com/cobaltspeech/diatheke/sdk-diatheke[-lite]`.
Please note:  Currently, the version is a variable set in the `lib/build.gradle`.  This should probably be refactored at some point.

### Cleaning

If you need to clean up the genereated code, etc. `./gradlew clean` is the command to run.
