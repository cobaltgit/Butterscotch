package net.perfectdreams.butterscotch.android

import android.content.Context
import android.net.Uri
import android.util.Log
import androidx.documentfile.provider.DocumentFile
import net.perfectdreams.butterscotch.android.library.GameLibrary
import java.io.File
import java.util.zip.ZipInputStream
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

object GameImporter {
    private const val TAG = "GameImporter"

    suspend fun importFolder(
        context: Context,
        treeUri: Uri,
        library: GameLibrary,
        writeFileCallback: (String) -> Unit
    ): GameLibrary.StagedGame? = withContext(Dispatchers.IO) {

        val root = DocumentFile.fromTreeUri(context, treeUri)

        if (root == null || !root.isDirectory) {
            return@withContext null
        }

        val staged = library.beginStaging()

        try {
            copyTree(
                context,
                root,
                staged.bundleDir,
                writeFileCallback
            )

            staged

        } catch (e: Exception) {
            Log.e(TAG, "Folder import failed", e)
            library.discardStaging(staged)
            null
        }
    }


    suspend fun importZip(
        context: Context,
        zipUri: Uri,
        library: GameLibrary,
        writeFileCallback: (String) -> Unit
    ): GameLibrary.StagedGame? = withContext(Dispatchers.IO) {

        val staged = library.beginStaging()

        try {
            extractZip(
                context,
                zipUri,
                staged.bundleDir,
                writeFileCallback
            )

            staged

        } catch (e: Exception) {
            Log.e(TAG, "ZIP import failed", e)
            library.discardStaging(staged)
            null
        }
    }


    private fun copyTree(
        context: Context,
        src: DocumentFile,
        dest: File,
        writeFileCallback: (String) -> Unit
    ) {
        if (!dest.exists())
            dest.mkdirs()

        for (child in src.listFiles()) {

            val name = child.name ?: continue
            val target = File(dest, name)

            if (child.isDirectory) {

                copyTree(
                    context,
                    child,
                    target,
                    writeFileCallback
                )

            } else if (child.isFile) {

                writeFileCallback(name)

                context.contentResolver
                    .openInputStream(child.uri)
                    ?.use { input ->
                        target.outputStream().use { output ->
                            input.copyTo(output)
                        }
                    }
            }
        }
    }


    private fun extractZip(
        context: Context,
        uri: Uri,
        dest: File,
        writeFileCallback: (String) -> Unit
    ) {

        val input =
            context.contentResolver.openInputStream(uri)
                ?: error("Unable to open ZIP")

        input.use {
            extractZipStream(
                it,
                dest,
                writeFileCallback
            )
        }
    }


    private fun extractZipStream(
        input: java.io.InputStream,
        dest: File,
        writeFileCallback: (String) -> Unit
    ) {

        ZipInputStream(input.buffered()).use { zip ->

            while (true) {

                val entry = zip.nextEntry ?: break

                val safeName =
                    entry.name.replace('\\', '/')

                if (safeName.contains("..")) {
                    zip.closeEntry()
                    continue
                }

                val target =
                    File(dest, safeName)

                if (entry.isDirectory) {

                    target.mkdirs()

                } else {

                    target.parentFile?.mkdirs()

                    writeFileCallback(
                        target.name
                    )

                    target.outputStream().use {
                        zip.copyTo(it)
                    }
                }

                zip.closeEntry()
            }
        }
    }
}
